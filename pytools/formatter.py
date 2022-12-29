import re
from enum import Enum
from itertools import groupby, count
from typing import Generator, TextIO

INDENT = " " * 4


class TokenType(Enum):
    EMPTY = 0
    COMMENT = 1
    SHARP_X = 2
    EQ = 3
    COMMA = 4
    LC = 5
    RC = 6
    ITEM = 7


class Token:
    def __init__(self, line_no: int, token_type: TokenType, value: str):
        self.line_no = line_no
        self.token_type = token_type
        self.value = value

    @property
    def comment(self):
        ret = self.value.lstrip(";").rstrip()
        if not ret.startswith(" "):
            ret = " " + ret
        return ";" + ret

    @classmethod
    def read(cls, io: TextIO) -> Generator["Token", None, None]:
        line_no = 0
        expect_eq = False
        while True:
            line = io.readline()
            if not line:
                break
            line_no += 1
            line = line.rstrip()
            if not line:
                yield cls(line_no, TokenType.EMPTY, "")
                continue
            while line:
                line = line.lstrip()
                if line.startswith(";"):
                    yield cls(line_no, TokenType.COMMENT, line)
                    line = ""
                elif line.startswith("#"):
                    eq_idx = line.find("=")
                    symbol = line[:eq_idx].strip()[1:2]
                    if symbol and symbol in 'QSGBFNq':
                        yield cls(line_no, TokenType.SHARP_X, line[:eq_idx])
                        line = line[eq_idx:]
                        expect_eq = True
                    else:
                        # treat as normal content
                        stop = re.search(r"[;,{}\s]", line)
                        if stop:
                            yield cls(line_no, TokenType.ITEM, line[: stop.start()])
                            line = line[stop.start():]
                        else:
                            yield cls(line_no, TokenType.ITEM, line)
                            line = ""
                elif line.startswith("=") and expect_eq:
                    yield cls(line_no, TokenType.EQ, line[:1])
                    line = line[1:]
                    expect_eq = False
                elif line.startswith(","):
                    yield cls(line_no, TokenType.COMMA, line[:1])
                    line = line[1:]
                elif line.startswith("{"):
                    yield cls(line_no, TokenType.LC, line[:1])
                    line = line[1:]
                elif line.startswith("}"):
                    yield cls(line_no, TokenType.RC, line[:1])
                    line = line[1:]
                elif line:
                    # yield until reach any of the above or whitespace
                    stop = re.search(r"[;#,{}\s]", line)
                    if stop:
                        yield cls(line_no, TokenType.ITEM, line[: stop.start()])
                        line = line[stop.start():]
                    else:
                        yield cls(line_no, TokenType.ITEM, line)
                        line = ""

    def __repr__(self):
        return f"Token({self.line_no}, {self.token_type}, {self.value!r})"


def get_line_tokens(f: TextIO):
    tokens = list(Token.read(f))
    # find patterns
    # 1. consecutive empty lines
    empty_lines = list(filter(lambda t: t.token_type == TokenType.EMPTY, tokens))
    line_no = [t.line_no for t in empty_lines]
    line_no = [list(g) for k, g in groupby(line_no, lambda x, c=count(): next(c) - x)]
    line_no = [g for g in line_no if len(g) > 1]
    expand_line_no = [item for lis in line_no for i, item in enumerate(lis) if i > 0]
    bad_lines = list(filter(lambda t: t.line_no in expand_line_no, tokens))

    # 2. one line definition (LC & RC on the same line)
    one_line_starts = []
    i = 0
    while i < len(tokens):
        if tokens[i].token_type == TokenType.LC:
            j = i + 1
            while j < len(tokens):
                if (
                    tokens[j].token_type == TokenType.RC
                    and tokens[j].line_no == tokens[i].line_no
                ):
                    one_line_starts.append(tokens[i])
                    i = j
                    break
                j += 1
        i += 1

    token_lines = []
    line = []
    visited = set()

    def flush_line():
        nonlocal line, token_lines
        if line:
            token_lines.append(line.copy())
            line.clear()

    i = 0
    while i < len(tokens):
        token = tokens[i]
        if token in bad_lines:
            i += 1
            continue
        elif token.token_type == TokenType.EMPTY:
            flush_line()
            line = [token]
            flush_line()
            i += 1
        elif token.token_type == TokenType.SHARP_X:
            flush_line()
            token.value = token.value.strip()
            # find the following EQ, maybe comments in between
            j = i + 1
            while j < len(tokens):
                if tokens[j].token_type == TokenType.EQ:
                    break
                j += 1
            # move comments to the beginning of SHARP_X line
            for c in filter(
                lambda t: t.token_type == TokenType.COMMENT, tokens[i + 1: j]
            ):
                token_lines.append([c])
            # find the following LC or ITEM, maybe comments in between
            k = j + 1
            while k < len(tokens):
                if tokens[k].token_type in (TokenType.LC, TokenType.ITEM):
                    break
                k += 1
            for c in filter(
                lambda t: t.token_type == TokenType.COMMENT, tokens[j + 1: k]
            ):
                token_lines.append([c])
            if tokens[k].token_type == TokenType.ITEM:
                # one line definition
                line.extend([token, tokens[j], tokens[k]])
                i = k + 1
            elif tokens[k] in one_line_starts:
                # one line definition
                line.extend([token, tokens[j], tokens[k]])
                x = k + 1
                while tokens[x].token_type != TokenType.RC:
                    assert tokens[x].token_type != TokenType.COMMENT
                    line.append(tokens[x])
                    x += 1
                line.append(tokens[x])
                i = x + 1
            else:  # multiline definition
                line.extend([token, tokens[j], tokens[k]])
                x = k + 1
                while tokens[x].token_type != TokenType.RC:
                    type_ = tokens[x].token_type
                    if type_ == TokenType.EMPTY:
                        x += 1
                        continue  # no empty lines in multiline definition
                    if type_ == TokenType.ITEM:
                        flush_line()
                        comma = Token(tokens[x].line_no, TokenType.COMMA, ",")
                        indent = Token(tokens[x].line_no, TokenType.EMPTY, INDENT)
                        line.extend([indent, tokens[x], comma])
                    elif type_ == TokenType.COMMENT:
                        line.append(tokens[x])
                    elif type_ == TokenType.COMMA:
                        pass  # discard
                    x += 1
                if line and line[-1].token_type == TokenType.COMMA:
                    line.pop()
                flush_line()
                line.append(tokens[x])
                i = x + 1
        elif token.token_type == TokenType.COMMENT:
            if line and line[-1].line_no == token.line_no:
                line.append(token)
            else:
                flush_line()
                line.append(token)
            i += 1
        else:
            if token.token_type != TokenType.ITEM:
                raise ValueError(f"Unexpected token: {token}")
            if token in visited:
                i += 1
                continue

            flush_line()
            # take next 5 item tokens
            items = [token]
            j = i + 1
            while j < len(tokens) and len(items) < 5:
                if tokens[j].token_type == TokenType.ITEM:
                    items.append(tokens[j])
                j += 1
            line.extend(items)
            visited.update(items)
            i += 1

    flush_line()
    return token_lines


def format(f: TextIO, strict: bool = True):
    token_lines = get_line_tokens(f)

    for i in reversed(range(len(token_lines))):
        line = token_lines[i]
        # merge consecutive comments into one
        while (
            len(line) >= 2
            and line[-1].token_type == TokenType.COMMENT
            and line[-2].token_type == TokenType.COMMENT
        ):
            line[-2].value += line.pop().value

    space_after_token = [TokenType.SHARP_X, TokenType.EQ]
    if not strict:
        space_after_token.append(TokenType.COMMA)
    comment_sp = " " * 2

    def_line_temp = {}  # (content, comment)
    trans_line_temp = {}  # (1,2,3,4,5,comment)
    for line_no, line in enumerate(token_lines):
        line_buf = ""
        if len(line) >= 5 and all(t.token_type == TokenType.ITEM for t in line[:5]):
            # translation line
            x = [t.value for t in line[:5]]
            if line[-1].token_type == TokenType.COMMENT:
                x.append(line[-1].comment)
            else:
                x.append("")
            trans_line_temp[line_no] = x
            continue

        for i, token in enumerate(line):
            if token.token_type == TokenType.COMMENT:
                def_line_temp[line_no] = (line_buf, token.comment)
                line_buf = ""
            else:
                line_buf += token.value
            if token.token_type in space_after_token:
                line_buf += " "
        if line_buf:
            if line_buf.rstrip():
                def_line_temp[line_no] = (line_buf.rstrip(), "")
            else:
                def_line_temp[line_no] = ("", "")

    # align transition lines
    widths = [0] * 5
    for a, b, c, d, e, comment in trans_line_temp.values():
        x = [a, b, c, d, e]
        widths = [max(w, len(s)) for w, s in zip(widths, x)]
    for line_no, (a, b, c, d, e, comment) in trans_line_temp.items():
        widths_ = widths.copy()
        if not comment:
            widths_[-1] = 0  # no comment, avoid extra space after last item
        if strict:
            widths_ = [0] * 5  # strict mode, no transition inner alignment
        def_line_temp[line_no] = (
            " ".join(s.ljust(w) for s, w in zip([a, b, c, d, e], widths_)),
            comment,
        )

    # align comments
    if all(not comment for _, comment in def_line_temp.values()):
        max_width = 0  # no comments, no need to align
    else:
        max_width = max(len(content) 
                        for content, comment in def_line_temp.values() 
                        if comment)
    lines = [""] * len(token_lines)
    for i, _ in enumerate(lines):
        if i not in def_line_temp:
            continue
        line, comment = def_line_temp[i]
        if comment and line:
            lines[i] = line.ljust(max_width) + comment_sp + comment
        elif comment or line:
            lines[i] = line or comment

    # add empty line between definitions
    for i in reversed(range(len(lines))):
        if lines[i].startswith("#") and not lines[i].endswith("{"):
            lines.insert(i + 1, "")
        elif lines[i].startswith("}"):
            lines.insert(i + 1, "")

    return "\n".join(lines).replace("\n\n\n", "\n\n")


def main():
    import argparse
    from pathlib import Path

    # 2 modes:
    # a. format a file, if output given, write to output, otherwise overwrite input
    # b. format files in a directory, if output given, write to output, otherwise overwrite input

    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="input file or directory", type=Path)
    parser.add_argument("-o", "--output", help="output file or directory", type=Path, default=None)
    parser.add_argument("-s", "--strict", help="strict mode", action="store_true")
    args = parser.parse_args()

    if args.input.is_dir():
        if args.output is not None and args.output.exists() and not args.output.is_dir():
            print("Output must be a directory if input is a directory")
            exit(1)
        if args.output is not None and not args.output.exists():
            args.output.mkdir(exist_ok=True, parents=True)
        for f in args.input.glob("*.tm"):
            formatted = format(f.open(), args.strict)
            if args.output is not None:
                args.output.joinpath(f.name).write_text(formatted)
            else:
                f.write_text(formatted)
    else:
        formatted = format(args.input.open(), args.strict)
        if args.output is not None:
            args.output.write_text(formatted)
        else:
            args.input.write_text(formatted)


if __name__ == "__main__":
    main()
