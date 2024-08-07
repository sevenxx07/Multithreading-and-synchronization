# Implementation

In the main thread (main() function), one thread is created, which we be called the producer and then N threads the consumer.
The value of N will be entered as a parameter when starting the program (argv[1]). If no parameter is specified, N will be equal to 1.
N must be between 1 and the number of CPUs in the system (sysconf(_SC_NPROCESSORS_ONLN)). If a different value is entered, the program will exit with a return code of 1.

The producer complies with the following:
* It reads from standard input “commands” in the form of <X> <word> pairs, where <X> is a non-negative integer and <word> is any non-empty sequence of characters (except whitespace).
  X is separated from the word by one space, individual commands are always separated from each other by the end of the line (\n). There may or may not be a newline (\n) at the end of the input.
* So a valid input might look like this:<br />
    &nbsp; &nbsp;20 foo<br />
    &nbsp; &nbsp;5 bar<br />
    &nbsp; &nbsp;1 base<br />
* The word length is limited only by the amount of available memory. This means that is needed to store the word in memory once. We may not have enough memory for multiple copies.
* If an invalid command (ie not matching the previous point) is entered, the program exits with a return code of 1, but only after the previous, valid commands have been processed.
* For each command read, it dynamically allocates (malloc(), new, …) a data structure, stores X and a word in it, and puts it at the end of the linked list.

Each consumer complies with the following:
* It selects items inserted by the producer from the beginning of the link list.
* If there is no item in the list, it waits for the producer to add something there (no consumption of computing time, no polling).
* If the producer adds P items, at most P consumers will wake up, the others will continue to wait.
* For each item picked up, the consumer writes the string "Thread n: word word word..." to the standard output, where n is the number of the consumer (the order of creation of the consumer in the range 1-N)
  and the word is repeated X times (information from the producer). This string will be all on one line terminated by \n.

# Example of input

input: (echo "20 foo"; echo "3 bar"; echo "5 baz") | ./prod-cons 4 <br />

will produce this output:<br />

Thread 1: foo foo foo foo foo foo foo foo foo foo foo foo foo foo foo foo foo foo foo foo<br />
Thread 2: bar bar bar<br />
Thread 1: baz baz baz baz baz<br />
<br />

input: echo "invalid" | ./prod-cons 4   echo "Exit code: $?"<br />
will produce this output:<br />

Exit code: 1
