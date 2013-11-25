eDSL
====

Currently nothing works. I'm in the process of implementing functionality and cleaning up code, etc etc.
    * Update: We can now successfully compile the lexer/parser combo into an executable!

Efforts have been made to anticipate future functionality without sacrificing current functionality.

This README will change significantly before its final edition to include a more concise language
description and usage guide. This information will be duplicated in the UPBOT wiki.

In the meantime, here is a rundown of what each file in this directory does.

QUICKSTART GUIDE
----------------

At minimum, you will need the following libraries for Janus to function properly:
  * gcc
  * flex
  * bison
  * make

This software is commonly provided via a package manager on GNU/Linux distributions.

If you are on a Mac/OSX system, it is strongly recommended that you install these libraries
via [HomeBrew](http://brew.sh/), a popular third-party package manager for OSX.

Windows users may look to Cygwin for these packages.


Once these packages are installed, you simply have to run the following commands to have
a Janus DSL interpreter.

```
make janus
./janus < INPUT_FILE
```

At the time of this writing, Janus only supports interpreting files via STDIN and outputting
files to STDOUT. Reading and writing from disk is planned for future versions of the interpreter.

Once the above commands are run you should see C source output to STDOUT that you can then compile
into the UPBOT system. Please refer to the UPBOT documentation for integration instructions.

FLEX
----

FLEX is a lexical analyzer that tokenizes files based on a set of rules defined in .l files.
For our purposes, this file is janus.l, which tokenizes and captures the vast majority of the
functionality that we plan on supporting.

BISON
-----

BISON is a semantic analyzer that will output generated code based upon a CFG that we define within
.y files. For our purposes this is janus.y, and again, attempts have been made to support functionality
that is not yet present in the core C libraries for UPBOT.


JANUS
------------

Janus is a simple Domain Specific Language intended to ease the development of state
machines used in UPBOT for both new Computer Science students and domain experts alike.

Janus is designed to succintly describe a given state machine that the developer would like UPBOT
to assume during a given trial. To this end, the syntax supports the definition of states in blocks, and
is compiled down to C for inclusion in the final UPBOT binary that is deployed.

Here is a sample state machine written in Janus:

```
begin state "zero"
    bump both -> turn random : "zero"
    time 5    -> drive med   : "one"
    othwerise -> drive low   : "zero"
end state

begin state "one"
    bump both -> turn random : "zero"
    time 100  -> stop now    : "stop"
    otherwise -> drive med   : "one"
end state

begin state "stop"
    nothing -> stop now : "stop"
end state
```

Let us take apart the first state definition in order to better understand Janus.

`begin state "zero" [...] end state"`

We begin with a statement that defines a block to describe our state, in this case state "zero".
"zero" is the unique _label_ used to identify the state. Each statement within the state block
will define the behavior that each state will encapsulate, as well as information on the next state.

`(bump both) -> (turn random) : "zero"`

We now arrive at the crux of of the UPBOT system: each state is defined by a set of event:responder pairs.
An event is a specific event that takes place, such as a bumped sensor, elapsed time, or other pertinent
data that is retrieved from the robot. A responder is the action that should take place in response to the
event. The final portion of this statement is another label, which is used to identify the state that
an event:responder pair should transition to. The above snippet thus states that when both sensors are bumped,
the robot will randomly turn and then yield to state zero.

We proceed through each statement linearly until we reach the base case.

`(otherwise) -> (drive med) : "zero"`

This statement is neither necessary nore required, it is simply sugar to define an event:responder pair
that yields to the same state. The above statement is simply the base action that will allow us to
drive at a medium speed if no other interesting events have ocurred since the last state. Another way
to express the "default" case is to use the `nothing` keyword, which is orthogonal to the `otherwise`
keyword.

In sum, state "zero" can be translated into English as follows:

"For state 'zero', bump both paired with drive med will yield state 'zero'."

TBC
