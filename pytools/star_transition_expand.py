import itertools
import re


def convert(transistions: str, alphabet: str):
    transition_list = transistions.splitlines()
    transition_parts = [
        transition.strip().split()
        for transition in filter(lambda x: bool(x.strip()), transition_list)
    ]
    original_state_order = {}
    existing_input = {}  # map from state to input
    final_transitions = []
    convert_transitions = {}  # map from state, input to transition
    expand_transitions = []
    for line, transition in enumerate(transition_parts):
        if len(transition) != 5:
            raise ValueError('invalid transition: ' + str(transition))
        input_ = transition[1]
        state = transition[0]
        if '*' not in input_:  # a deterministic transition
            existing_input.setdefault(state, set()).add(input_)
            final_transitions.append((transition, False))
        else:
            convert_transitions.setdefault((state, input_), []).append(transition)
            final_transitions.append((transition, True))
        if state not in original_state_order:
            original_state_order[state] = line

    for (state, star_input_), transitions in convert_transitions.items():
        if len(transitions) > 1:
            raise ValueError('multiple transitions for ' + str((state, star_input_)))
        transition = transitions[0]
        n = len(star_input_)
        inputs = [''.join(x) for x in itertools.product(alphabet, repeat=n)]
        for input_ in inputs:
            # check satisfies star_input_
            if not all(x == '*' or x == y for x, y in zip(star_input_, input_)):
                continue
            # check not already exists
            if input_ in existing_input.get(state, set()):
                continue
            # replace * in output (index 2) according to input
            output = transition[2]
            for i, (u, v, w) in enumerate(zip(star_input_, input_, output)):
                if w == '*':
                    output = output[:i] + v + output[i + 1 :]
            expand_transitions.append(
                [state, input_, output, transition[3], transition[4]]
            )
            existing_input.setdefault(state, set()).add(input_)
    # group by state
    # final_transitions.sort(key=lambda x: original_state_order[x[0]])
    return expand_transitions, final_transitions


def get_alphabet(input_: str) -> str:
    # pattern: #G = {a, b, c, ...}
    # or
    # #G = { [multiple lines] }
    pattern = re.compile(r'#G\s*=\s*{\s*(.+?)\s*}', re.DOTALL)
    match = pattern.search(input_)
    if not match:
        raise ValueError('no alphabet found')
    alphabet = match.group(1)
    symbols = re.split(r'\s*,\s*', alphabet)
    return ''.join(symbols)


def get_n_tapes(input_: str) -> int:
    # pattern: #N = \d+
    pattern = re.compile(r'#N\s*=\s*(\d+)', re.DOTALL)
    return int(pattern.search(input_).group(1))


def get_transitions(input_: str, n: int) -> str:
    # pattern: state \ws input \ws output \ws direction \ws next_state
    #          do not match comma or semicolon
    state = r'[a-zA-Z0-9_]+'
    direction = r'[lr\*]+'
    # find lines <state> <input: n> <output: n> <direction: n> <next_state>
    pattern = re.compile(
        r'({state})\s+({input})\s+({output})\s+({direction})\s+({state})'.format(
            state=state,
            input=r'[^,;]' + '{{{}}}'.format(n),
            output=r'[^,;]' + '{{{}}}'.format(n),
            direction=direction,
        ),
        re.DOTALL,
    )
    match = pattern.findall(input_)
    return '\n'.join(' '.join(x) for x in match if len(x) == 5)


def make_line(transition: list, is_star: bool = False) -> str:
    if is_star:
        return '; ' + ' '.join(transition)
    else:
        return ' '.join(transition)


def main():
    from argparse import ArgumentParser
    import sys

    parser = ArgumentParser(
        description='This script expands transitions with "*" to multiple transitions in a turing machine definition file (.tm) '
        'by replacing "*" with all possible symbols in the alphabet.'
    )
    parser.add_argument('input', type=str)
    parser.add_argument('-o', '--output', type=str)
    args = parser.parse_args()

    with open(args.input, 'r') as f:
        input_ = f.read()
    alphabet = get_alphabet(input_)
    n = get_n_tapes(input_)
    transitions = get_transitions(input_, n)
    expand, final = convert(transitions, alphabet)
    if args.output:
        f = open(args.output, 'w')
    else:
        f = sys.stdout
    print('\n'.join(map(lambda x: make_line(x[0], x[1]), final)), file=f)
    print('; program expanded transitions', file=f)
    print('\n'.join(map(lambda x: make_line(x), expand)), file=f)


main()
