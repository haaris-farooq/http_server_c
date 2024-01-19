# Simple Multithreaded HTTP Server in C

This project is a lightweight, multithreaded HTTP server written in pure C. It is designed with simplicity and readability in mind and meant to enhance my understanding of web server architecture.

## Compilation

To compile the project, you can use either CMake or the provided `build.sh` script, which utilizes the GCC compiler. Ensure you have GCC installed on your system. To stop the server gracefully, you can use SIGINT (Ctrl+C) or run the `stop_server.sh` script.

## Project Overview

This server utilizes a thread pool to create a scalable infrastructure. The key goals of the project are:

- [x] **Static Hosting**

- [ ] **Serving website from local files**

- [x] **Multithreaded** 

- [x] **Thread Pool**

- [ ] **HTTP 1.0 Support For All Requests** 

**Note:** This project is intended for educational purposes and is not recommended for hosting real websites.



