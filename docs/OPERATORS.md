# PostScript Operators Reference

This document provides a comprehensive reference for all PostScript operators implemented in the interpreter.

## Table of Contents

1. [Stack Operations](#stack-operations)
2. [Arithmetic Operations](#arithmetic-operations)
3. [Comparison Operations](#comparison-operations)
4. [Control Flow](#control-flow)
5. [Array Operations](#array-operations)
6. [Dictionary Operations](#dictionary-operations)
7. [Graphics Operations](#graphics-operations)
8. [String Operations](#string-operations)
9. [Type Conversion](#type-conversion)
10. [System Operations](#system-operations)

## Stack Operations

### Basic Stack Manipulation

#### `dup`

- **Syntax**: `any dup`
- **Description**: Duplicates the top element on the operand stack
- **Stack**: `any` → `any any`
- **Example**:
  ```postscript
  5 dup    % Stack: 5 5
  ```

#### `pop`

- **Syntax**: `any pop`
- **Description**: Removes the top element from the operand stack
- **Stack**: `any` → (empty)
- **Example**:
  ```postscript
  5 3 pop  % Stack: 5
  ```

#### `exch`

- **Syntax**: `any1 any2 exch`
- **Description**: Exchanges the top two elements on the operand stack
- **Stack**: `any1 any2` → `any2 any1`
- **Example**:
  ```postscript
  5 3 exch  % Stack: 3 5
  ```

#### `clear`

- **Syntax**: `... clear`
- **Description**: Removes all elements from the operand stack
- **Stack**: `any*` → (empty)
- **Example**:
  ```postscript
  1 2 3 clear  % Stack: empty
  ```

### Advanced Stack Operations

#### `roll`

- **Syntax**: `... objn ... obj0 n j roll`
- **Description**: Rolls j elements up n positions on the stack
- **Stack**: `objn ... obj0 n j` → `obj(j-1) ... obj0 objn ... objj`
- **Example**:
  ```postscript
  1 2 3 4 4 2 roll  % Stack: 3 4 1 2
  ```

#### `index`

- **Syntax**: `... anyn ... any0 n index`
- **Description**: Copies the nth element from the top of the stack to the top
- **Stack**: `anyn ... any0 n` → `anyn ... any0 anyn`
- **Example**:
  ```postscript
  1 2 3 4 2 index  % Stack: 1 2 3 4 2
  ```

#### `copy`

- **Syntax**: `... anyn ... any0 n copy`
- **Description**: Copies the top n elements on the stack
- **Stack**: `anyn ... any0 n` → `anyn ... any0 anyn ... any0`
- **Example**:
  ```postscript
  1 2 3 3 copy  % Stack: 1 2 3 1 2 3
  ```

## Arithmetic Operations

### Basic Arithmetic

#### `add`

- **Syntax**: `num1 num2 add`
- **Description**: Adds two numbers
- **Stack**: `num1 num2` → `sum`
- **Example**:
  ```postscript
  5 3 add  % Result: 8
  ```

#### `sub`

- **Syntax**: `num1 num2 sub`
- **Description**: Subtracts num2 from num1
- **Stack**: `num1 num2` → `difference`
- **Example**:
  ```postscript
  5 3 sub  % Result: 2
  ```

#### `mul`

- **Syntax**: `num1 num2 mul`
- **Description**: Multiplies two numbers
- **Stack**: `num1 num2` → `product`
- **Example**:
  ```postscript
  5 3 mul  % Result: 15
  ```

#### `div`

- **Syntax**: `num1 num2 div`
- **Description**: Divides num1 by num2
- **Stack**: `num1 num2` → `quotient`
- **Example**:
  ```postscript
  6 2 div  % Result: 3
  ```

#### `mod`

- **Syntax**: `int1 int2 mod`
- **Description**: Returns the remainder of int1 divided by int2
- **Stack**: `int1 int2` → `remainder`
- **Example**:
  ```postscript
  7 3 mod  % Result: 1
  ```

### Mathematical Functions

#### `neg`

- **Syntax**: `num neg`
- **Description**: Negates a number
- **Stack**: `num` → `-num`
- **Example**:
  ```postscript
  5 neg  % Result: -5
  ```

#### `abs`

- **Syntax**: `num abs`
- **Description**: Returns the absolute value of a number
- **Stack**: `num` → `|num|`
- **Example**:
  ```postscript
  -5 abs  % Result: 5
  ```

#### `floor`

- **Syntax**: `num floor`
- **Description**: Returns the largest integer less than or equal to num
- **Stack**: `num` → `floor(num)`
- **Example**:
  ```postscript
  3.7 floor  % Result: 3
  ```

#### `ceiling`

- **Syntax**: `num ceiling`
- **Description**: Returns the smallest integer greater than or equal to num
- **Stack**: `num` → `ceiling(num)`
- **Example**:
  ```postscript
  3.2 ceiling  % Result: 4
  ```

#### `round`

- **Syntax**: `num round`
- **Description**: Rounds a number to the nearest integer
- **Stack**: `num` → `round(num)`
- **Example**:
  ```postscript
  3.6 round  % Result: 4
  ```

## Comparison Operations

### Equality Comparisons

#### `eq`

- **Syntax**: `any1 any2 eq`
- **Description**: Tests if two objects are equal
- **Stack**: `any1 any2` → `bool`
- **Example**:
  ```postscript
  5 5 eq    % Result: true
  5 3 eq    % Result: false
  ```

#### `ne`

- **Syntax**: `any1 any2 ne`
- **Description**: Tests if two objects are not equal
- **Stack**: `any1 any2` → `bool`
- **Example**:
  ```postscript
  5 3 ne    % Result: true
  5 5 ne    % Result: false
  ```

### Numeric Comparisons

#### `gt`

- **Syntax**: `num1 num2 gt`
- **Description**: Tests if num1 is greater than num2
- **Stack**: `num1 num2` → `bool`
- **Example**:
  ```postscript
  5 3 gt    % Result: true
  3 5 gt    % Result: false
  ```

#### `lt`

- **Syntax**: `num1 num2 lt`
- **Description**: Tests if num1 is less than num2
- **Stack**: `num1 num2` → `bool`
- **Example**:
  ```postscript
  3 5 lt    % Result: true
  5 3 lt    % Result: false
  ```

#### `ge`

- **Syntax**: `num1 num2 ge`
- **Description**: Tests if num1 is greater than or equal to num2
- **Stack**: `num1 num2` → `bool`
- **Example**:
  ```postscript
  5 5 ge    % Result: true
  5 3 ge    % Result: true
  3 5 ge    % Result: false
  ```

#### `le`

- **Syntax**: `num1 num2 le`
- **Description**: Tests if num1 is less than or equal to num2
- **Stack**: `num1 num2` → `bool`
- **Example**:
  ```postscript
  5 5 le    % Result: true
  3 5 le    % Result: true
  5 3 le    % Result: false
  ```

## Control Flow

### Conditional Execution

#### `if`

- **Syntax**: `bool proc if`
- **Description**: Executes proc if bool is true
- **Stack**: `bool proc` → (empty)
- **Example**:
  ```postscript
  true { (Hello) } if
  false { (Hello) } if  % Nothing happens
  ```

#### `ifelse`

- **Syntax**: `bool proc1 proc2 ifelse`
- **Description**: Executes proc1 if bool is true, proc2 if false
- **Stack**: `bool proc1 proc2` → (empty)
- **Example**:
  ```postscript
  true { (Yes) } { (No) } ifelse  % Prints: Yes
  false { (Yes) } { (No) } ifelse % Prints: No
  ```

### Loops

#### `for`

- **Syntax**: `initial increment limit proc for`
- **Description**: Executes proc for each value from initial to limit by increment
- **Stack**: `initial increment limit proc` → (empty)
- **Example**:
  ```postscript
  0 1 5 { dup . } for  % Prints: 0 1 2 3 4 5
  ```

#### `repeat`

- **Syntax**: `int proc repeat`
- **Description**: Executes proc int times
- **Stack**: `int proc` → (empty)
- **Example**:
  ```postscript
  3 { (Hello) } repeat  % Prints: Hello Hello Hello
  ```

#### `while`

- **Syntax**: `proc1 proc2 while`
- **Description**: Executes proc2 while proc1 returns true
- **Stack**: `proc1 proc2` → (empty)
- **Example**:
  ```postscript
  { dup 5 lt } { dup 1 add } while  % Adds 1 until >= 5
  ```

### Procedure Control

#### `exec`

- **Syntax**: `proc exec`
- **Description**: Executes a procedure
- **Stack**: `proc` → (empty)
- **Example**:
  ```postscript
  { 5 3 add } exec  % Result: 8
  ```

## Array Operations

### Array Creation and Access

#### `array`

- **Syntax**: `int array`
- **Description**: Creates an array of length int
- **Stack**: `int` → `array`
- **Example**:
  ```postscript
  3 array  % Creates array of length 3
  ```

#### `length`

- **Syntax**: `array length`
- **Description**: Returns the length of an array
- **Stack**: `array` → `int`
- **Example**:
  ```postscript
  [1 2 3] length  % Result: 3
  ```

#### `get`

- **Syntax**: `array index get`
- **Description**: Gets the element at index in array
- **Stack**: `array index` → `element`
- **Example**:
  ```postscript
  [1 2 3] 1 get  % Result: 2
  ```

#### `put`

- **Syntax**: `array index value put`
- **Description**: Puts value at index in array
- **Stack**: `array index value` → `array`
- **Example**:
  ```postscript
  [1 2 3] 1 5 put  % Result: [1 5 3]
  ```

### Array Manipulation

#### `aload`

- **Syntax**: `array aload`
- **Description**: Pushes all elements of array onto the stack
- **Stack**: `array` → `array elem0 ... elem(n-1)`
- **Example**:
  ```postscript
  [1 2 3] aload  % Stack: [1 2 3] 1 2 3
  ```

#### `astore`

- **Syntax**: `array elem0 ... elem(n-1) astore`
- **Description**: Stores elements into array
- **Stack**: `array elem0 ... elem(n-1)` → `array`
- **Example**:
  ```postscript
  [null null null] 1 2 3 astore  % Result: [1 2 3]
  ```

## Dictionary Operations

### Dictionary Creation and Access

#### `dict`

- **Syntax**: `int dict`
- **Description**: Creates a dictionary with capacity int
- **Stack**: `int` → `dict`
- **Example**:
  ```postscript
  10 dict  % Creates dictionary with capacity 10
  ```

#### `begin`

- **Syntax**: `dict begin`
- **Description**: Pushes dict onto the dictionary stack
- **Stack**: `dict` → (empty)
- **Example**:
  ```postscript
  << /a 1 >> begin
  ```

#### `end`

- **Syntax**: `end`
- **Description**: Pops the top dictionary from the dictionary stack
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  end  % Pops current dictionary
  ```

#### `def`

- **Syntax**: `key value def`
- **Description**: Associates key with value in current dictionary
- **Stack**: `key value` → (empty)
- **Example**:
  ```postscript
  /x 5 def  % Defines x as 5
  ```

#### `load`

- **Syntax**: `key load`
- **Description**: Pushes the value associated with key onto the stack
- **Stack**: `key` → `value`
- **Example**:
  ```postscript
  /x load  % Pushes value of x
  ```

#### `store`

- **Syntax**: `key value store`
- **Description**: Stores value for key in current dictionary
- **Stack**: `key value` → (empty)
- **Example**:
  ```postscript
  /x 10 store  % Stores 10 for x
  ```

### Dictionary Information

#### `known`

- **Syntax**: `key known`
- **Description**: Tests if key is defined in current dictionary
- **Stack**: `key` → `bool`
- **Example**:
  ```postscript
  /x known  % Returns true if x is defined
  ```

#### `where`

- **Syntax**: `key where`
- **Description**: Returns the dictionary containing key
- **Stack**: `key` → `dict bool`
- **Example**:
  ```postscript
  /x where  % Returns dict and true if found
  ```

## Graphics Operations

### Path Construction

#### `moveto`

- **Syntax**: `x y moveto`
- **Description**: Moves the current point to (x, y)
- **Stack**: `x y` → (empty)
- **Example**:
  ```postscript
  100 100 moveto  % Move to point (100, 100)
  ```

#### `lineto`

- **Syntax**: `x y lineto`
- **Description**: Adds a line from current point to (x, y)
- **Stack**: `x y` → (empty)
- **Example**:
  ```postscript
  200 200 lineto  % Line to point (200, 200)
  ```

#### `rlineto`

- **Syntax**: `dx dy rlineto`
- **Description**: Adds a line from current point by relative offset (dx, dy)
- **Stack**: `dx dy` → (empty)
- **Example**:
  ```postscript
  50 0 rlineto  % Line 50 units to the right
  ```

#### `curveto`

- **Syntax**: `x1 y1 x2 y2 x3 y3 curveto`
- **Description**: Adds a Bézier curve
- **Stack**: `x1 y1 x2 y2 x3 y3` → (empty)
- **Example**:
  ```postscript
  100 100 200 100 200 200 curveto
  ```

#### `closepath`

- **Syntax**: `closepath`
- **Description**: Closes the current path
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  closepath  % Close current path
  ```

### Painting

#### `stroke`

- **Syntax**: `stroke`
- **Description**: Paints the current path with the current stroke color
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  stroke  % Paint current path
  ```

#### `fill`

- **Syntax**: `fill`
- **Description**: Fills the current path with the current fill color
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  fill  % Fill current path
  ```

#### `eofill`

- **Syntax**: `eofill`
- **Description**: Fills the current path using even-odd rule
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  eofill  % Fill using even-odd rule
  ```

### Graphics State

#### `gsave`

- **Syntax**: `gsave`
- **Description**: Saves the current graphics state
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  gsave  % Save graphics state
  ```

#### `grestore`

- **Syntax**: `grestore`
- **Description**: Restores the graphics state
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  grestore  % Restore graphics state
  ```

#### `setlinewidth`

- **Syntax**: `num setlinewidth`
- **Description**: Sets the line width
- **Stack**: `num` → (empty)
- **Example**:
  ```postscript
  2 setlinewidth  % Set line width to 2
  ```

#### `setrgbcolor`

- **Syntax**: `red green blue setrgbcolor`
- **Description**: Sets the RGB color (0.0 to 1.0)
- **Stack**: `red green blue` → (empty)
- **Example**:
  ```postscript
  1 0 0 setrgbcolor  % Set color to red
  ```

#### `setgray`

- **Syntax**: `gray setgray`
- **Description**: Sets the gray level (0.0 to 1.0)
- **Stack**: `gray` → (empty)
- **Example**:
  ```postscript
  0.5 setgray  % Set to 50% gray
  ```

### Transformations

#### `translate`

- **Syntax**: `tx ty translate`
- **Description**: Translates the coordinate system by (tx, ty)
- **Stack**: `tx ty` → (empty)
- **Example**:
  ```postscript
  100 100 translate  % Move origin by (100, 100)
  ```

#### `scale`

- **Syntax**: `sx sy scale`
- **Description**: Scales the coordinate system by (sx, sy)
- **Stack**: `sx sy` → (empty)
- **Example**:
  ```postscript
  2 2 scale  % Scale by factor of 2
  ```

#### `rotate`

- **Syntax**: `angle rotate`
- **Description**: Rotates the coordinate system by angle degrees
- **Stack**: `angle` → (empty)
- **Example**:
  ```postscript
  45 rotate  % Rotate 45 degrees
  ```

#### `concat`

- **Syntax**: `a b c d e f concat`
- **Description**: Concatenates a transformation matrix
- **Stack**: `a b c d e f` → (empty)
- **Example**:
  ```postscript
  1 0 0 1 100 100 concat  % Translate by (100, 100)
  ```

### Text Operations

#### `findfont`

- **Syntax**: `fontname findfont`
- **Description**: Finds a font by name
- **Stack**: `fontname` → `font`
- **Example**:
  ```postscript
  /Times-Roman findfont  % Get Times-Roman font
  ```

#### `scalefont`

- **Syntax**: `font size scalefont`
- **Description**: Scales a font to the specified size
- **Stack**: `font size` → `font`
- **Example**:
  ```postscript
  /Times-Roman findfont 12 scalefont  % 12pt Times-Roman
  ```

#### `setfont`

- **Syntax**: `font setfont`
- **Description**: Sets the current font
- **Stack**: `font` → (empty)
- **Example**:
  ```postscript
  /Times-Roman findfont 12 scalefont setfont
  ```

#### `show`

- **Syntax**: `string show`
- **Description**: Shows a string at the current point
- **Stack**: `string` → (empty)
- **Example**:
  ```postscript
  (Hello, World!) show
  ```

#### `stringwidth`

- **Syntax**: `string stringwidth`
- **Description**: Returns the width of a string in the current font
- **Stack**: `string` → `width`
- **Example**:
  ```postscript
  (Hello) stringwidth  % Returns width of "Hello"
  ```

### Page Operations

#### `showpage`

- **Syntax**: `showpage`
- **Description**: Outputs the current page
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  showpage  % Output current page
  ```

#### `copypage`

- **Syntax**: `copypage`
- **Description**: Copies the current page
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  copypage  % Copy current page
  ```

## String Operations

### String Creation and Access

#### `string`

- **Syntax**: `int string`
- **Description**: Creates a string of length int
- **Stack**: `int` → `string`
- **Example**:
  ```postscript
  10 string  % Creates string of length 10
  ```

#### `length`

- **Syntax**: `string length`
- **Description**: Returns the length of a string
- **Stack**: `string` → `int`
- **Example**:
  ```postscript
  (Hello) length  % Result: 5
  ```

#### `get`

- **Syntax**: `string index get`
- **Description**: Gets the character at index in string
- **Stack**: `string index` → `char`
- **Example**:
  ```postscript
  (Hello) 1 get  % Result: 101 (ASCII 'e')
  ```

#### `put`

- **Syntax**: `string index char put`
- **Description**: Puts character at index in string
- **Stack**: `string index char` → `string`
- **Example**:
  ```postscript
  (Hello) 1 120 put  % Result: (Hxllo)
  ```

### String Manipulation

#### `dup`

- **Syntax**: `string dup`
- **Description**: Duplicates a string
- **Stack**: `string` → `string string`
- **Example**:
  ```postscript
  (Hello) dup  % Stack: (Hello) (Hello)
  ```

#### `copy`

- **Syntax**: `string1 string2 copy`
- **Description**: Copies string1 to string2
- **Stack**: `string1 string2` → `string2`
- **Example**:
  ```postscript
  (Hello) (     ) copy  % Result: (Hello)
  ```

## Type Conversion

### Type Checking

#### `type`

- **Syntax**: `any type`
- **Description**: Returns the type of an object
- **Stack**: `any` → `nametype`
- **Example**:
  ```postscript
  5 type        % Result: /integertype
  (Hello) type  % Result: /stringtype
  ```

#### `cvlit`

- **Syntax**: `any cvlit`
- **Description**: Converts to literal
- **Stack**: `any` → `any`
- **Example**:
  ```postscript
  /x cvlit  % Makes x literal
  ```

#### `cvi`

- **Syntax**: `any cvi`
- **Description**: Converts to integer
- **Stack**: `any` → `int`
- **Example**:
  ```postscript
  3.7 cvi  % Result: 3
  ```

#### `cvr`

- **Syntax**: `any cvr`
- **Description**: Converts to real
- **Stack**: `any` → `real`
- **Example**:
  ```postscript
  5 cvr  % Result: 5.0
  ```

#### `cvn`

- **Syntax**: `any cvn`
- **Description**: Converts to name
- **Stack**: `any` → `name`
- **Example**:
  ```postscript
  (x) cvn  % Result: /x
  ```

#### `cvs`

- **Syntax**: `any string cvs`
- **Description**: Converts to string
- **Stack**: `any string` → `string`
- **Example**:
  ```postscript
  123 (   ) cvs  % Result: (123)
  ```

## System Operations

### Output and Display

#### `print`

- **Syntax**: `any print`
- **Description**: Prints an object to standard output
- **Stack**: `any` → (empty)
- **Example**:
  ```postscript
  (Hello) print  % Prints: Hello
  ```

#### `pstack`

- **Syntax**: `pstack`
- **Description**: Prints the operand stack
- **Stack**: (unchanged)
- **Example**:
  ```postscript
  1 2 3 pstack  % Prints stack contents
  ```

#### `stack`

- **Syntax**: `stack`
- **Description**: Prints the operand stack without changing it
- **Stack**: (unchanged)
- **Example**:
  ```postscript
  1 2 3 stack  % Prints stack contents
  ```

### Error Handling

#### `stop`

- **Syntax**: `stop`
- **Description**: Stops execution
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  stop  % Stops execution
  ```

#### `quit`

- **Syntax**: `quit`
- **Description**: Exits the interpreter
- **Stack**: (empty) → (empty)
- **Example**:
  ```postscript
  quit  % Exit interpreter
  ```

### Debugging

#### `==`

- **Syntax**: `any ==`
- **Description**: Prints an object in readable form
- **Stack**: `any` → (empty)
- **Example**:
  ```postscript
  [1 2 3] ==  % Prints: [1 2 3]
  ```

#### `=`

- **Syntax**: `any =`
- **Description**: Prints an object and a newline
- **Stack**: `any` → (empty)
- **Example**:
  ```postscript
  5 =  % Prints: 5
  ```

---

This reference covers all the PostScript operators currently implemented in the interpreter. For more information about specific operators or implementation details, see the source code in the `src/interpreter/ps_operators.cpp` file.
