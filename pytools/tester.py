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
        tik = time.perf_counter()
        for _ in range(n):
            input_, expect = self.rand_testcase()
            mem += self.test(input_, expect)
        tok = time.perf_counter()
        print(f'{n} tests passed, {tok - tik:.3f} seconds elapsed, {mem / n:.3f} MB per test')

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
            print('input: ', input_)
            print('expect:', repr(expect))
            print('output:', repr(output))
            raise AssertionError('test failed')
        return mem

    @staticmethod
    def get_memory_usage(pid: int) -> float:
        import concurrent.futures

        def f():
            max_mem = 0
            while psutil.pid_exists(pid):
                try:
                    mem = psutil.Process(pid).memory_info().rss
                except psutil.NoSuchProcess:
                    break
                max_mem = max(max_mem, mem)
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