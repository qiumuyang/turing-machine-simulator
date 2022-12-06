from importlib import import_module
from pathlib import Path

from tester import Testcase

tests = [
    ('test_addition', 'binary_addition.tm', True),
    ('test_aibj', 'double_aibj_checker.tm', True),
    ('test_circular_shift', 'right_circular_shift_expand.tm', True),
    ('test_div', 'binary_ceiling_div2.tm', True),
    ('test_multiplication', 'multiplication_checker.tm', True),
    ('test_parenthesis', 'parenthesis_checker_expand.tm', True),
    ('test_strlen', 'strlen_expand.tm', True),
]
tests_wildcard = [
    ('test_circular_shift', 'right_circular_shift.tm', True),
    ('test_parenthesis', 'parenthesis_checker.tm', True),
    ('test_perfect_square', 'perfect_square_checker.tm', True),
    ('test_strlen', 'strlen.tm', True),
]
tests_nijika = [
    ('test_nijika_bin2dec', 'bin2dec.tm', True),
    ('test_nijika_binaryadd', 'binaryadd.tm', True),
    ('test_nijika_binarymult', 'binarymult.tm', True),
    ('test_nijika_concatenate', 'concatenate.tm', True),
    ('test_nijika_gcd', 'gcd.tm', True),
    ('test_nijika_parentheses', 'parentheses.tm', True),
    ('test_nijika_primetest', 'primetest.tm', True),
    ('test_nijika_reversepolishboolean', 'reversepolishboolean.tm', True),
]

tm_dir = Path(__file__).parent.parent / 'examples'
nijika_dir = Path(__file__).parent.parent / 'nijika'

suits = [
    (lambda x: True, tests, tm_dir),
    (lambda x: x.wildcard, tests_wildcard, tm_dir),
    (lambda x: x.nijika, tests_nijika, nijika_dir),
]

test_cnt = 0


def test(executable: str, py_testcase: str, tm_file: str, n: int, base_dir: Path) -> None:
    global test_cnt

    module = import_module(py_testcase)
    for _, cls in module.__dict__.items():
        if isinstance(cls, type) and issubclass(cls, Testcase) and cls is not Testcase:
            break
    else:
        print(f'no Testcase found in {py_testcase}')
        return
    test_cnt += 1
    print(f'[{test_cnt}]', py_testcase, tm_file)
    cls(executable, str(base_dir / tm_file)).test_many(n)


def main():
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('exec')
    parser.add_argument('-n', type=int, default=10)
    parser.add_argument(
        '-w',
        '--wildcard',
        action='store_true',
        help='use testcases with wildcard transition',
    )
    parser.add_argument(
        '-e',
        '--nijika',
        action='store_true',
        help='use nijika testcases (see https://github.com/NijikaIjichi/FLA-Testcases)',
    )
    args = parser.parse_args()
    for condition, tests, base_dir in suits:
        if not condition(args):
            continue
        for py_testcase, tm_file, enabled in tests:
            if enabled:
                test(args.exec, py_testcase, tm_file, args.n, base_dir)


if __name__ == '__main__':
    main()
