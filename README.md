# lightweight_json

This is a simple C99 JSON library which serializes data directly to a char buffer with a given size, calling a flush callback if necessary.
It also supports deserializing strings containing JSON, although the input has to be complete.

## Supported Types
- Strings
- uint64_t
- int64_t
- double (will be serialized with 8 decimal point precision)
- Objects (-> nesting)
- Arrays consisting of the above datatypes

## Nesting
The default maximum nesting is 10, but can be modified by defining `LIGHTWEIGHT_JSON_MAX_NESTING_SIZE` before building the code.

## Caveats
Little testing and not perfect error handling when passing in broken JSON data.
When reading, you have to know the contents of the JSON file, i.e. you cannot fetch key names, what their value type is and then the value.
For reading you need the complete string, there is no chunkwise reading.

## Documentation / Example Code
The header file is rather small and the functions are documented, feel free to read through it.
There is also a small example program at `src/testprog.c`.
