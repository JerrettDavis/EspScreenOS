# Contributing

Run before opening a pull request:

```bash
python tools/espscreen.py validate --all
python tools/espscreen.py generate --all --check
python tools/espscreen.py test
```

A board change must include its source evidence, generated output, and hardware qualification status. A new supported board must be added to the exact Python test matrix and firmware workflow.

Keep application code board-independent. New shared abstractions belong in the narrowest component that can own them. Avoid heap-backed registries, exceptions, RTTI, silent pin sharing, and unbounded queues.

Use conventional commit subjects where practical. Pull requests should explain the invariant being protected, test evidence, hardware revision when relevant, and rollback implications.
