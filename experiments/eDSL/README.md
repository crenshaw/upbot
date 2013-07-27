eDSL
====

Currently nothing works. I'm in the process of implementing functionality and cleaning up code, etc etc.

Efforts have been made to anticipate future functionality without sacrificing current functionality.

This README will change significantly before its final edition to include a more concise language
description and usage guide. This information will be duplicated in the UPBOT wiki.

In the meantime, here is a rundown of what each file in this directory does.

FLEX
----

FLEX is a lexical analyzer that tokenizes files based on a set of rules defined in .l files.
For our purposes, this file is robot.l, which tokenizes and captures the vast majority of the
functionality that we plan on supporting.

BISON
-----

BISON is a semantic analyzer that will output generated code based upon a CFE that we define within
.y files. For our purposes this is robot.y, and again, attempts have been made to support functionality
that is not yet present in the core C libraries for UPBOT.


[ROBOT LANG]
------------
We should probably come up with a name for this fancy new language. I am open to suggestions, and will update
this README with a list of viable options for a final decision at some point in the next month.

Moving on. The [ROBOT LANG] is a simple Domain Specific Language intended to ease the development of state
machines used in UPBOT for both new Computer Science students and domain experts alike.

[ROBOT LANG] is designed to succintly describe a given state machine that the developer would like UPBOT
to assume during a given trial. To this end, the syntax supports the definition of states in blocks, and
is compiled down to C for inclusion in the final UPBOT binary that is deployed.

Here is a sample state machine written in [ROBOT LANG]:

```
begin state "zero"
    (bump both) -> (turn random) : "zero"
    (time 5)    -> (drive med)   : "one"
    (othwerise) -> (drive low)   : "zero"
end state

begin state "one"
    (bump both) -> (turn random) : "zero"
    (time 100)  -> (stop now)    : "stop"
    (otherwise) -> (drive med)   : "one"
end state

begin state "stop"
    (nothing) -> (stop now) : "stop"
end state
```

Let us take apart the first state definition in order to better understand [ROBOT LANG].

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
