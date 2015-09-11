### =================== TESTS =================== ###

INCLUDE(CTest)

SET(CTEST_PROJECT_NAME "LibChaos")
SET(CTEST_DROP_METHOD "http")
SET(CTEST_DROP_SITE "localhost")
SET(CTEST_DROP_LOCATION "/cdash/submit.php?project=LibChaos")
SET(CTEST_DROP_SITE_CDASH TRUE)
SET(CTEST_TRIGGER_SITE "localhost")

ENABLE_TESTING()
