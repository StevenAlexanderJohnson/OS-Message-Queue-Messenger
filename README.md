# OS-Message-Queue-Messenger
This is a project I took to experiment with message queues. This was developed in on a Linux system.

When launched this program will spawn a child process that reads the messages sent by the other client. The parent process is in charge of sending the messages.
Note that it is important to pay attention the message when you launch the program. It will tell you to use 'Ctrl+D' to close your application. If you don't the message queue that is opened by the process will not be closed properly and will remain open until either you close it yourself, or restart your computer.
