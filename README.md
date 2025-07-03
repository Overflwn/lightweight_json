# lightweight_json

This is a simple C99 JSON serializer which serializes data directly to a char buffer.

## Supported Types
- Strings
- uint64_t
- int64_t
- double (will be serialized with 8 decimal point precision)

## Nesting
The default maximum nesting is 10, but can be modified by defining `LIGHTWEIGHT_JSON_MAX_NESTING_SIZE` before including the header file.

## Caveats
No de-serialization. Might be added later.

## Documentation / Example Code
The header file is rather small and the functions are documented, feel free to read through it.
There is also a small example program at `src/testprog.c`.
