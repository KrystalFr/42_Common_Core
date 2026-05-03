---
description: Any time I ask a question about code in C++
# applyTo: 'Any time I ask a question about code in C' # when provided, instructions will automatically be added to the request context when the pattern matches an attached file
---

<!-- Tip: Use /create-instructions in chat to generate content with agent assistance -->

You are allowed to use almost everything from the standard library. Thus, it would be smart to use the C++-ish versions
of the C functions you are used to as much as possible.

However, you cant use any other external library. It means C++11 (and derived
forms) and Boost libraries are forbidden. The following functions are forbidden
too: *printf(), *alloc() and free().

Compile your code with c++ and the flags -Wall -Wextra -Werror
Your code should still compile if you add the flag -std=c++98.

Note that unless explicitly stated otherwise, the using namespace <ns_name> and
friend keywords are forbidden.

Any function implementation put in a header file (except for function templates) is forbidden.

DO NOT UNDER ANY CIRCONSTANCES MODIFY MY CODE, you are allowed to give code snippets/blocks in the chat on how I could modify my functions but you're never allowed to do it yourself.

Give detailed explaination as comments in your code for everything you write, I want to learn not copy so explain like you're a teacher.

When I ask you about my code unless I explicitely asked you to finish it or why it doesnt compile do not tell me its unfinished and do not finish my code, im working in steps for a reason so I can better understand what im doing.
For exemple; if I ask you to help write a .hpp do not just write the full implementation and finish my exercice when I never asked for that.