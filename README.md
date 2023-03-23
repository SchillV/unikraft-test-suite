# unikraft-test-suite
A test suite for unikraft, including test scripts, an overall automation script and other miscellaneous scripts

ALL TEST CASE FILES ARE TAKEN FROM THE LINUX TEST PROJECT.
# Running the test suite
In order to run the test suite, execute the "runtest" script, found in the folder named 'independent tests'.
A github access token is needed in order for kraft to work properly.
To enable the GUI, use the -g parameter

Logs are found in the appropriately named folder, and contain any error messages given by the tests.

# Expanding the test suite
All of the tests found in the syscalls folder are partly imported, using the convert.py script.
In order for the automation script to work properly, two conditions need to be met:
- The test should output "test succeeded" if it works
- If the system call folder has more than one test, a header file named "incl.h" should be created

If the two conditions are met, then the results should update by simply running the 'runtest' script

Some general rules of thumb:
- Usually the proprietary ltp functions have a standard c counterpart
- I found it easier to have the original test opened in parallel (they can be found in the "original tests" folder)
- Some system calls can be called dirrectly as a function (it will throw a compiler warning), while others can be accessed using the syscall.h library. This can be determined by checking the system call man page
- Some test files can not feasibly be made to work independently of the ltp framework

# The GUI
The automation script has the option to output the results in a graphical output. This is represented by a number of stacked bar charts for every system call. Green represends tests that pass, red represents tests that fail and gray represents unfinished or broken tests. The rightmost plot represents the total for all of the test files.

Note: the current test script relies on the unikraft efi loader. This feature is currently not finished. The tests are executed inside the docker container.