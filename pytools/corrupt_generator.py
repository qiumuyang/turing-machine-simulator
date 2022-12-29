from copy import deepcopy
import random
import string


def duplicate(set_: list) -> list:
    if set_:
        return [random.choice(set_)] + set_
    else:
        return []


def corrupt(func):
    def wrapper(*args, **kwargs):
        generator: CorruptGenerator = args[0]
        if generator.corrupt_type == func.__name__ and not generator.done:
            generator.done = True
            return func(*args, **kwargs)
        else:
            name = func.__name__ + '_'
            return getattr(generator, name)(*args[1:], **kwargs)  # remove self

    return wrapper


class CorruptGenerator:
    def __init__(self, corrupt_type: str) -> None:
        self.corrupt_type = corrupt_type
        self.done = False

    @corrupt
    def duplicate_state(self, states: list) -> list:
        return duplicate(states)

    def duplicate_state_(self, states: list) -> list:
        return states

    @corrupt
    def invalid_state_name(self, states: list) -> list:
        state = random.choice(states)
        state = random.choice('!@$%^&') + state
        return states + [state]

    def invalid_state_name_(self, states: list) -> list:
        return states

    @corrupt
    def duplicate_transition(self, transitions: list) -> list:
        return duplicate(transitions)

    def duplicate_transition_(self, transitions: list) -> list:
        return transitions

    @corrupt
    def duplicate_alphabet(self, alphabet: list) -> list:
        return duplicate(alphabet)

    def duplicate_alphabet_(self, alphabet: list) -> list:
        return alphabet

    @corrupt
    def undefined_state(self, states: list) -> str:
        return random.choice(states) + '_'

    def undefined_state_(self, states: list) -> str:
        return random.choice(states)

    @corrupt
    def undefined_symbol(self, alphabet: list) -> str:
        return random.choice([x for x in string.printable if x not in alphabet])

    def undefined_symbol_(self, alphabet: list) -> str:
        return random.choice(alphabet)

    @corrupt
    def missing_brackets(self, set_: list) -> str:
        return ', '.join(set_)

    def missing_brackets_(self, set_: list) -> str:
        return '{' + ', '.join(set_) + '}'

    @corrupt
    def extra_brackets(self, item: str) -> str:
        return '{' + item + '}'

    def extra_brackets_(self, item: str) -> str:
        return item

    @corrupt
    def missing_component(self, components: dict) -> dict:
        key = random.choice(list(components.keys()))
        components.pop(key)
        return components

    def missing_component_(self, components: dict) -> str:
        return components

    @corrupt
    def extra_component(self) -> dict:
        return {'unknown': 'unknown'}

    def extra_component_(self) -> dict:
        return {}

    @corrupt
    def invalid_tape_number(self, tape_number: int) -> str:
        return str(tape_number) + 'a'

    def invalid_tape_number_(self, tape_number: int) -> str:
        return str(tape_number)

    @corrupt
    def missing_eq(self, s: str) -> str:
        indices = [i for i, x in enumerate(s) if x == '=']
        miss = random.choice(indices)
        return s[:miss] + s[miss + 1 :]

    def missing_eq_(self, s: str) -> str:
        return s

    @corrupt
    def missing_comma(self, s: str) -> str:
        indices = [i for i, x in enumerate(s) if x == ',']
        miss = random.choice(indices)
        return s[:miss] + s[miss + 1 :]

    def missing_comma_(self, s: str) -> str:
        return s

    @corrupt
    def corrupt_transition(self, transitions: list) -> list:
        idx = random.randint(0, len(transitions) - 1)
        parts = transitions[idx].split()
        part_id = random.randint(0, len(parts) - 1)
        parts.pop(part_id)
        transitions[idx] = ' '.join(parts)
        return transitions

    def corrupt_transition_(self, transitions: list) -> list:
        return transitions

    @corrupt
    def unmatch_tape_number(self, transitions: list) -> list:
        idx = random.randint(0, len(transitions) - 1)
        parts = transitions[idx].split()
        incr = random.randint(1, 3)
        pos = random.randint(1, 3)  # input / output / direction
        parts[pos] += random.choice(parts[pos]) * incr
        transitions[idx] = ' '.join(parts)
        return transitions

    def unmatch_tape_number_(self, transitions: list) -> list:
        return transitions

    @classmethod
    def all_corruptions(cls) -> list:
        _corrupt = cls('duplicate_state')  # dummy
        names = [x for x in dir(_corrupt) if x + '_' in dir(_corrupt)]
        return names


class TM:
    def __init__(self, states, alphabet, transitions, tapes: int) -> None:
        self.states = states
        self.transitions = transitions
        self.alphabet = alphabet
        self.tapes = tapes

    def to_string(self, corrupt: CorruptGenerator) -> str:
        states = corrupt.duplicate_state(self.states)
        states = corrupt.invalid_state_name(states)
        transitions = deepcopy(corrupt.duplicate_transition(self.transitions))
        alphabet = corrupt.duplicate_alphabet(self.alphabet)
        blank = corrupt.undefined_symbol(alphabet)
        init = corrupt.undefined_state(states)
        final = [corrupt.undefined_state(states)]
        tape_number = corrupt.invalid_tape_number(self.tapes)

        components = {
            'Q': corrupt.missing_brackets(states),
            'S': corrupt.missing_brackets(alphabet),
            'G': corrupt.missing_brackets(alphabet),
            'B': corrupt.extra_brackets(blank),
            'q0': corrupt.extra_brackets(init),
            'F': corrupt.missing_brackets(final),
            'N': tape_number,
        }
        components = corrupt.missing_component(components)
        components.update(corrupt.extra_component())
        keys = [k for k in 'QSGF' if k in components and ',' in components[k]]
        key = random.choice(keys)
        components[key] = corrupt.missing_comma(components[key])

        head = '\n'.join([f'#{k} = {v}' for k, v in components.items()])
        head = corrupt.missing_eq(head)
        transitions = corrupt.corrupt_transition(transitions)
        transitions = corrupt.unmatch_tape_number(transitions)
        return head + '\n' + '\n'.join(transitions)


def main(executable: str, temp_dir: str):
    import tempfile
    import subprocess
    import os

    tm = TM(
        states=['q0', 'q1', 'q2'],
        alphabet=['0', '1'],
        transitions=['q0 0 1 r q1', 'q1 1 1 r q2', 'q2 0 0 r q2', 'q2 1 1 r q2'],
        tapes=1,
    )
    if temp_dir and not os.path.exists(temp_dir):
        os.makedirs(temp_dir)
    for corrupt_type in CorruptGenerator.all_corruptions():
        corrupt = CorruptGenerator(corrupt_type)
        if temp_dir:
            tmp = os.path.join(temp_dir, corrupt_type)
            delete = False
        else:
            tmp = tempfile.mktemp()
            delete = True
        with open(tmp, mode='w') as f:
            f.write(tm.to_string(corrupt))
            f.flush()
        proc = subprocess.Popen([executable, f.name, '-v'], stderr=subprocess.PIPE)
        _, err = proc.communicate()
        print(f'{corrupt_type}:\n{err.decode()}')
        if delete:
            os.remove(tmp)


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('executable', help='path to the executable')
    parser.add_argument('-t', '--temp', help='path to the temporary directory', default='')
    args = parser.parse_args()

    main(args.executable, args.temp)
