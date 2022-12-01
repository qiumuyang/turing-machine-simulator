import platform
import subprocess
import time
import psutil


class Testcase:
    def __init__(self, executable: str, tm_file: str):
        self.executable = executable
        self.tm_file = tm_file

    def rand_testcase(self) -> tuple:
        raise NotImplementedError

    def test_many(self, n: int) -> None:
        mem = 0
        elapsed = 0
        passed = 0
        for _ in range(n):
            input_, expect = self.rand_testcase()
            try:
                tik = time.perf_counter()
                mem += self.test(input_, expect)
                tok = time.perf_counter()
                passed += 1
                elapsed += tok - tik
            except AssertionError:
                pass
        n = passed
        print(
            f'{n} tests passed, {elapsed:.3f} seconds elapsed, {mem / max(n, 1):.3f} MB per test'
        )

    def test(self, input_: str, expect: str) -> float:
        proc = subprocess.Popen(
            [self.executable, self.tm_file, input_], stdout=subprocess.PIPE, shell=False
        )
        mem = Testcase.get_memory_usage(proc.pid)
        if isinstance(expect, bool):
            expect = 'true' if expect else 'false'
        elif not isinstance(expect, str):
            expect = str(expect)
        output = proc.stdout.read().decode('ascii').strip()
        if output != expect:
            print('[FAIL]')
            print('input: ', repr(input_))
            print('output:', repr(output))
            print('expect:', repr(expect))
            raise AssertionError('test failed')
        return mem

    @staticmethod
    def get_memory_usage(pid: int) -> float:
        import concurrent.futures

        query_zombie = lambda: psutil.Process(pid).status() == psutil.STATUS_ZOMBIE
        if platform.system() == 'Windows':
            query_zombie = lambda: False

        def f():
            max_mem = 0
            try:
                while True:
                    mem = psutil.Process(pid).memory_info().rss
                    max_mem = max(max_mem, mem)
                    if query_zombie():
                        break
            except psutil.NoSuchProcess:
                pass
            return max_mem  # in bytes

        with concurrent.futures.ThreadPoolExecutor(max_workers=1) as executor:
            future = executor.submit(f)
            return future.result() / 1024 / 1024  # in MB

    @classmethod
    def parse_args(cls):
        import argparse

        parser = argparse.ArgumentParser()
        parser.add_argument('exec')
        parser.add_argument('tm')
        parser.add_argument('-n', type=int, default=1)
        args = parser.parse_args()
        cls(args.exec, args.tm).test_many(args.n)
