from importlib import import_module
from pathlib import Path

from tester import Testcase

tests = [
    ('test_addition', 'binary_addition.tm'),
    ('test_aibj', 'double_aibj_checker.tm'),
    ('test_circular_shift', 'right_circular_shift_expand.tm'),
    ('test_div', 'binary_ceiling_div2.tm'),
    ('test_multiplication', 'multiplication_checker.tm'),
    ('test_parenthesis', 'parenthesis_checker_expand.tm'),
    ('test_strlen', 'strlen_expand.tm'),
]
tests_wildcard = [
    ('test_circular_shift', 'right_circular_shift.tm'),
    ('test_parenthesis', 'parenthesis_checker.tm'),
    ('test_strlen', 'strlen.tm'),
]
tm_dir = Path(__file__).parent.parent / 'examples'


test_cnt = 0


def test(executable: str, py_testcase: str, tm_file: str, n: int) -> None:
    global test_cnt
    test_cnt += 1

    module = import_module(py_testcase)
    for _, cls in module.__dict__.items():
        if isinstance(cls, type) and issubclass(cls, Testcase) and cls is not Testcase:
            break
    else:
        print(f'no Testcase found in {py_testcase}')
        return
    print(f'[{test_cnt}]', py_testcase, tm_file)
    cls(executable, str(tm_dir / tm_file)).test_many(n)


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
    args = parser.parse_args()
    for py_testcase, tm_file in tests:
        test(args.exec, py_testcase, tm_file, args.n)
    if args.wildcard:
        for py_testcase, tm_file in tests_wildcard:
            test(args.exec, py_testcase, tm_file, args.n)


if __name__ == '__main__':
    main()
