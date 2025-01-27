| [Home](/trick) → [Documentation Home](../Documentation-Home) → [Simulation Capabilities](Simulation-Capabilities) → Checkpoints |
|------------------------------------------------------------------|

The Trick CheckPointWriter is a C++ Class that implements checkpointing.

## Checkpointing
```python
# Save a checkpoint before the initialization jobs execute. default False
trick.checkpoint_pre_init(True|False)
# Save a checkpoint after the initialization jobs execute. default False
trick.checkpoint_post_init(True|False)
# Save a checkpoint when the simulation ends. default False
trick.checkpoint_end(True|False)
# Save a checkpoint at a time in the future
trick.checkpoint(<time>)
# Save a checkpoint at a time in the future to a specified file
trick.checkpoint(<time>, <file_name>)
# Save a checkpoint now
trick.checkpoint()
# Save a checkpoint now to a specified file
trick.checkpoint(<file_name>)

# Set the CPU to use for checkpoints
trick.checkpoint_cpu(<cpu_num>)

# Save a checkpoint periodically during simulation execution. default False
trick.checkpoint_safestore_set_enabled(True|False)
# Set the safestore checkpoint period. default 9x10e18
trick.checkpoint_safestore(<period>)

# Load a checkpoint
trick.load_checkpoint(<filename>)
# Load a checkpoint without restoring STLs
trick.load_checkpoint(<filename>, False)

```

[Continue to Memory Manager](memory_manager/MemoryManager)
