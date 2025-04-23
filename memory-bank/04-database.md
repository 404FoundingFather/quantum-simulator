# Data Management

**Last Updated:** April 19, 2025

This project does not use a traditional database. Instead, it uses file-based persistence for configuration and optional checkpointing of simulation state.

## Configuration Persistence

* **Format:** JSON (recommended) or HDF5
* **File:** `config/default_config.json`
* **Contents:** Simulation parameters (grid size, Δt, potential parameters, wavepacket parameters, physics constants)
* **Usage:** Loaded at startup by `config::ConfigLoader` and applied to `PhysicsConfig`.

## Checkpointing & Export

* **Checkpoint Files:** HDF5 files containing serialized `Wavefunction` grid (`std::vector<std::complex<double>>`), current time, and `PhysicsConfig`.
* **Location:** `checkpoints/` directory
* **Naming Convention:** `checkpoint_t{timestamp}.h5`
* **Use Case:** Pause and resume long-running simulations, post-processing analysis.

## Time-Series Data Export

* **Format:** CSV or JSON
* **Observables:** Total probability, expectation values, energy
* **File:** `output/observables.csv` or `output/observables.json`
* **Columns/Fields:** `time, total_probability, expectation_x, expectation_y, energy`

## Data Access

* **ConfigLoader (config/ConfigLoader.h)**: Parses JSON or HDF5 config files into `PhysicsConfig`.
* **CheckpointSerializer (config/CheckpointSerializer.h)**: Serializes and deserializes checkpoint files.
* **DataExporter (config/DataExporter.h)**: Writes observables to CSV/JSON.

## Backup and Recovery

* Checkpoint files can be used to recover simulation state after a crash or for branching experiments.
* Ensure enough disk space for HDF5 checkpoints (each ~16 MB per 512×512 grid).