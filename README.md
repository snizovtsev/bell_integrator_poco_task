# Test Task – Embedded Application Developer role

## Objective:
With the POCO library (https://pocoproject.org) write a simple TCP server that will accept a connection on a port 28888 that will reverse a string entered by client.

So on a Linux host after I run your application I can do 

```
$ telnet 28888
> Welcome to POCO TCP server. Enter you string:
Input by a person
<abcdefg
Output by server
>gfedcba
>12345b
<b54321
Etc.. The line is not to exceed 255 symbols.
```

## What need to be delivered:
The archive with the source code and instruction how to build and run the code on a Linux host.

## Build instruction

1. Install Bazel using [official instructions](https://bazel.build/install/).
2. Build and run using `bazel run //:tcp_reflector`
3. Connect to the server: `telnet localhost 28888`

Poco doesn't have to be installed on a target system.

## Caveats

I haven't time to test how errors are handled. Poco documentation says that receive/send may throw TimeoutException or NetException. The application may crash or leak memory in appearence of such events.
