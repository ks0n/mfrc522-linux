// SPDX-License-Identifier: GPL-2.0+

#include <kunit/test.h>

static void always_fail(struct kunit *test)
{
	KUNIT_FAIL(test, "[MFRC522] Always fails");
}

static struct kunit_case mfrc522_test_cases[] = { KUNIT_CASE(always_fail), {} };

static struct kunit_suite mfrc522_test_suite = {
	.name = "mfrc522-test-suite",
	.test_cases = mfrc522_test_cases,
};

kunit_test_suite(mfrc522_test_suite);
