#include <stdlib.h>
#include <check.h>

extern void add_testcases_url(Suite *);

int main() {
  int number_failed;
  Suite *s = suite_create("Netembryo");

  add_testcases_url(s);

  SRunner *sr = srunner_create (s);
  srunner_run_all (sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed (sr);
  srunner_free (sr);
  
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;  
}
