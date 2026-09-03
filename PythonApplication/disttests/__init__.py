# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

import importlib.machinery
import os
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from typing import Optional


class TestPythonInterpreter(unittest.TestCase):
    def test_all_spam_methods(self):
        import spam
        assert spam is not None
        self.assertEqual(2, spam.div(6,3))
        self.assertEqual(-3, spam.add(-1,-2))
        # Returns 9223372036854775807 on 64-bit systems
        MAX_INT64 = sys.maxsize 
        self.assertEqual(MAX_INT64, spam.inc(MAX_INT64 - 1))
        self.assertEqual("Hello, Alex!", spam.say_hello("Alex"))

    def test_no_overflow_from_spam(self):
       import spam
       MAX_INT64 = sys.maxsize 
       spam.inc(MAX_INT64)
       self.assertEqual(-MAX_INT64 -1, spam.inc(MAX_INT64))

    def test_type_from_spam(self):
        with self.assertRaises(TypeError):
            import spam
            spam.inc(0.0)
 
    def test_compression(self):
        import bz2
        import lzma
        import zlib

        self.assertTrue(lzma.is_check_supported(lzma.CHECK_CRC64))
        self.assertTrue(lzma.is_check_supported(lzma.CHECK_SHA256))

        bz2.compress(b"test")
        zlib.compress(b"test")


    @unittest.skipIf(
        sys.version_info[:2] < (3, 13),
        "Free-threaded builds are only available in 3.13+",
    )
    def test_gil_disabled(self):
        import sysconfig

        if "freethreaded" in os.environ.get("BUILD_OPTIONS", "").split("+"):
            wanted = 1
        else:
            wanted = 0

        self.assertEqual(sysconfig.get_config_var("Py_GIL_DISABLED"), wanted)


    @unittest.skipIf(
        sys.version_info[:2] < (3, 14),
        "zstd is only available in 3.14+",
    )
    def test_zstd_multithreaded(self):
        from compression import zstd  # type: ignore

        max_threads = zstd.CompressionParameter.nb_workers.bounds()[1]
        assert max_threads > 0, (
            "Expected multithreading to be enabled but max threads is zero"
        )



if __name__ == "__main__":
    unittest.main()
