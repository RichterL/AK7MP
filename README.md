# calculator

developed on Elementary OS 6 (ubuntu-based linux)

![screenshot](https://raw.githubusercontent.com/RichterL/AK7MP/master/screenshot.png)

Supports
- basic math operations (addition, subtraction, division, multiplication, exponents)
- operations with parentheses
- negative and decimal numbers
- result chaining (result of previous operation can be used directly as operand of next operation)
- follows BEDMAS left-to-right `8 / 2 * ( 2 + 2 ) = 16`

The calculator keeps entered operands and operations on internal stacks and immediately uses Reverse Polish Notation (postfix) parser to store operands and operators in output queue. When equals button is pressed, the RPN is calculated.
