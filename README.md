# 🐠 Fish Tool  
### The Jellyfish AI Development & Training Utility by **Fossil Logic**

**Fish Tool** is a specialized command-line utility dedicated to building, training, inspecting, and managing **Jellyfish AI modules and datasets**. It provides powerful model lifecycle commands, dataset workflows, auditing tools, and AI-assisted testing features.

Where **Shark** manages the filesystem, **Fish** manages the *intelligence*.

---

# 🎛 Command Palette Overview

## 🧬 AI Development Commands (Model Lifecycle)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|------------------|
| `create` | Create a new AI module, dataset, profile, or training plan. | `-t, --type <module/dataset/profile>`<br>`-n, --name <id>`<br>`--config <file>`<br>`--template <id>` |
| `delete` | Permanently remove modules, datasets, or checkpoints. | `-f, --force`<br>`-i, --interactive`<br>`--prune` Remove orphaned metadata |
| `introspect` | Inspect internal structure of a model or dataset. | `--weights` Show tensor stats<br>`--layers` Architecture<br>`--tokens` Token stats<br>`--verify` Validate integrity |
| `train` | Train or fine-tune a Jellyfish AI module using a dataset. | `-d, --dataset <path>`<br>`--epochs <n>`<br>`--batch <n>`<br>`--lr <rate>`<br>`--resume <checkpoint>`<br>`--save <path>` |
| `audit` | Analyze model behavior for safety, bias, drift, or anomalies. | `--drift`<br>`--bias`<br>`--toxicity`<br>`--explain`<br>`--export <path>` |
| `show` | Display information about modules, datasets, or metadata. | `-a, --all`<br>`--stats`<br>`--meta`<br>`--tags` |
| `view` | View raw dataset entries, tokens, or training samples. | `-n, --number <count>`<br>`--sample` Random selection<br>`--shuffle` |
| `export` | Export a model, dataset, or training result. Supports AI-centric formats for interoperability and deployment. | `-f, --format <fson/onnx/tensor>` Export format: `fson` (Fossil AI JSON), `onnx` (Open Neural Network Exchange), `tensor` (raw tensor weights)<br>`--dest <path>` Output file or directory<br>`--compress` Compress output<br>`--include-metadata` Include model/dataset metadata<br>`--overwrite` Overwrite existing files<br>`--quiet` Suppress output |
| `import` | Import a model, dataset, or configuration from supported AI formats. | `-f, --format <fson/onnx/tensor>` Input format<br>`--source <path>` File or directory to import<br>`--validate` Validate structure and integrity<br>`--name <id>` Assign module/dataset name<br>`--replace` Overwrite existing entry if it exists<br>`--quiet` Suppress output |
| `imagine` | Generate new content, modules, or data samples using Jellyfish AI. Ideal for prototyping, synthetic data creation, or creative outputs. | `-m, --model <id>` Select the model to use<br>`-p, --prompt <text>` Input prompt for generation<br>`--type` (`fson`, `json`, `yaml`, `toml`, `markdown`, `html`, `xml`, `csv`, `text`, `ini`) Output type<br>`--length <n>` Max output length<br>`--count <n>` Number of outputs to generate<br>`--seed <n>` Reproducible generation<br>`--save <path>` Save output to file<br>`--temperature <value>` Creativity/variation control |
| `dataset` | Dataset-focused operations (subcommands). | `add`, `remove`, `tag`, `clean`, `stats`, `split`, `verify` |
| `merge` | Merge datasets or modules. | `-s, --strategy <union/overwrite/append>`<br>`--dry-run` |
| `rebase` | Apply a dataset or config change onto an existing model. | `--config <file>`<br>`--align` |
| `prune` | Remove unused or low-value parameters, entries, or metadata. | `--small`<br>`--redundant`<br>`--orphans` |
| `decay` | Apply weight decay or gradual forgetting to models. | `--strength <value>`<br>`--simulate` |
| `seek` | AI-enhanced search across datasets or models. | `--query <text>`<br>`--semantic`<br>`--regex` |

---

## 🧪 AI Testing Commands

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|------------------|
| `ask` | Run a one-shot prompt against a module. | `-m, --model <id>`<br>`-f, --file <path>`<br>`--explain`<br>`--quiet` |
| `chat` | Interactive conversation session with a local module. | `--context` Keep history<br>`--save <file>` Save transcript<br>`-m, --model <id>` |
| `summary` | Summarize datasets, logs, files, or model states. | `-f, --file <path>`<br>`--depth <n>`<br>`--color`<br>`--time` |

---

## 🧩 Dataset Subcommands (`fish dataset`)

| **Subcommand** | **Description** | **Flags** |
|----------------|-----------------|-----------|
| `add` | Add new data samples (text, json, binary). | `--label <tag>`<br>`--source <id>` |
| `remove` | Delete dataset items. | `--id <range>`<br>`--filter <expr>` |
| `tag` | Apply internal tags to dataset entries. | `--add <tag>`<br>`--remove <tag>` |
| `clean` | Sanitize dataset entries (dedupe, normalize). | `--dedupe`<br>`--normalize`<br>`--strip-html` |
| `stats` | Show analytics and distribution stats. | `--tokens`<br>`--lengths`<br>`--labels` |
| `split` | Train/test/validation splitting. | `--train <pct>`<br>`--test <pct>`<br>`--seed <n>` |
| `verify` | Validate dataset structure and format. | `--strict` |

---

## 🌍 Global Flags (All Commands)

| **Flag** | **Description** |
|----------|-----------------|
| `--help` | Show help for a command or subcommand. |
| `--version` | Display Fish Tool version. |
| `-v, --verbose` | Enable detailed diagnostic output. |
| `-q, --quiet` | Suppress normal output. |
| `--color` | Enable colored output. |
| `--dry-run` | Simulate actions without making changes. |
| `--time` | Include timestamps in output. |

---

# 🐟 Usage Examples

| **Example** | **Description** |
|-------------|-----------------|
| `fish create -t module -n classifier --template base_small` | Create a new model using a template. |
| `fish train -m classifier -d data/train --epochs 5 --save out/checkpoints` | Train a model with a dataset. |
| `fish dataset clean --dedupe --normalize` | Clean and normalize dataset entries. |
| `fish introspect classifier --layers --weights` | Inspect model architecture and weight statistics. |
| `fish audit classifier --bias --toxicity --export audit.json` | Run a full audit and export results. |
| `fish merge dataset_A dataset_B --strategy union` | Merge multiple datasets into one. |
| `fish ask -m classifier "Explain this error code"` | Test a model with a one-shot question. |
| `fish summary -f data.fson --depth 3` | Summarize a dataset or file. |

## 🪼 Sample Jellyfish model

This sample shows how Jellyfish AI uses FSON to package each reasoning step as a Git-like block containing identity, timing, classification, I/O data, and trust attributes. By storing all this in a typed, structured format, a .fish file captures not just a model, but its entire reasoning trail—making AI behavior transparent, auditable, and reproducible.

```fson
{
  version: i32: 1,
  model_type: cstr: "jellyfish",
  chains: array: [
    cstr: "chains/chain_00001.fson",
    cstr: "chains/chain_00002.fson"
  ],
  records: cstr: "records.fson"
}
```

```fson
{
  history: array: [
    object: {
      timestamp: i64: 1700000000,
      event: cstr: "Model initialized",
      version: i32: 1,
      description: cstr: "Created model.fson skeleton"
    },
    object: {
      timestamp: i64: 1700000500,
      event: cstr: "First chain added",
      version: i32: 2,
      description: cstr: "chains/chain_00001.fson included"
    }
  ]
}
```

```fson
{
  chain: i32: 1,
  commit_count: i32: 2,
  commits: array: [
    object: {
      hash: cstr: "aa11bb22",
      timestamp: i64: 1700000123,
      input: object: {
        text: cstr: "What is 2+2?",
        tokens: array: [
          cstr: "what",
          cstr: "is",
          cstr: "2+2"
        ]
      },
      output: object: {
        text: cstr: "4",
        tokens: array: [
          cstr: "4"
        ]
      },
      confidence: f64: 0.95,
      valid: bool: true
    },

    object: {
      hash: cstr: "cc33dd44",
      timestamp: i64: 1700000150,
      input: object: {
        text: cstr: "Capital of France?",
        tokens: array: [
          cstr: "capital",
          cstr: "of",
          cstr: "france"
        ]
      },
      output: object: {
        text: cstr: "Paris",
        tokens: array: [
          cstr: "paris"
        ]
      },
      confidence: f64: 0.89,
      valid: bool: true
    }
  ]
}
```

## **Prerequisites**

Ensure you have the following installed before starting:

- **Meson Build System**: This project relies on Meson. For installation instructions, visit the official [Meson website](https://mesonbuild.com/Getting-meson.html).

## **Setting Up Meson Build**

1. **Install Meson**:
    - Follow the installation guide on the [Meson website](https://mesonbuild.com/Getting-meson.html) for your operating system.

## **Setting Up, Compiling, Installing, and Running the Project**

1. **Clone the Repository**:

    ```sh
    git clone https://github.com/fossillogic/fish.git
    cd fish
    ```

2. **Configure the Build**:

    ```sh
    meson setup builddir
    ```

3. **Compile the Project**:

    ```sh
    meson compile -C builddir
    ```

4. **Install the Project**:

    ```sh
    meson install -C builddir
    ```

5. **Run the Project**:

    ```sh
    fish
    ```

## **Contributing**

Interested in contributing? Please open pull requests or create issues on the [GitHub repository](https://github.com/fossillogic/fossil-app).

## **Feedback and Support**

For issues, questions, or feedback, open an issue on the [GitHub repository](https://github.com/fossillogic/fossil-app/issues).

## **License**

This project is licensed under the [Mozilla Public License](LICENSE).
