DESCRIPTION
===========

Parse and import the output of a Brocade device's `configshow -all` command.
Currently, this tool supports HP's Brocade FOS type devices.


EXAMPLES
========

Process the target data for the device `switch` from a local file.
```
nmdb-import-brocade --device-id switch configuration.txt
```

Assuming `...` is some command chain which retrieves the target data from a
remote host and displays the results locally, then the following would process
it and save the data to a file called `configuration.txt` in the current working
directory.
```
... | nmdb-import-brocade --device-id switch configuration.txt --pipe
```
