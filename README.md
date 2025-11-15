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
| `export` | Export a model, dataset, or training result. | `-f, --format <json/fson/bin/onnx/tensor>`<br>`--dest <path>` |
| `import` | Import a dataset, module, or configuration. | `--validate`<br>`--name <id>`<br>`--replace` |
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
| `fish summary -f data.json --depth 3` | Summarize a dataset or file. |

## Sample AI

A small, illustrative dataset for basic arithmetic operations. Designed for testing and training lightweight AI reasoning models.

```json
{
  name: cstr: "ArithmeticModule",
  type: cstr: "module",
  description: cstr: "AI model trained on SimpleMathDataset for arithmetic reasoning",
  version: cstr: "v1.0",
  base_dataset: cstr: "SimpleMathDataset",
  architecture: object: {
    layers: array: [
      object: { type: cstr: "Dense", units: i32: 16, activation: cstr: "relu" },
      object: { type: cstr: "Dense", units: i32: 8, activation: cstr: "relu" },
      object: { type: cstr: "Dense", units: i32: 1, activation: cstr: "linear" }
    ],
    optimizer: cstr: "adam",
    loss: cstr: "mean_squared_error"
  },
  training: object: {
    epochs: i32: 5,
    batch_size: i32: 2,
    metrics: array: [cstr: "accuracy"]
  },
  checkpoints: array: [
    object: { epoch: i32: 1, loss: f32: 0.125, hash: cstr: "abc123" },
    object: { epoch: i32: 5, loss: f32: 0.012, hash: cstr: "def456" }
  ],
  tags: array: [cstr: "arithmetic", cstr: "module", cstr: "trained"],
  semantic_meta: object: {
    confidence: f32: 0.95,
    trained_on_blocks: array: [i32: 0, i32: 1, i32: 2],
    reasoning_depth: i32: 2
  }
}
```

A small dataset for basic arithmetic operations, intended for testing or training AI reasoning models.

```json
{
  name: cstr: "SimpleMathDataset",
  type: cstr: "dataset",
  description: cstr: "A small dataset for basic arithmetic operations",
  version: cstr: "v1.0",
  samples: array: [
    object: {
      input: cstr: "2 + 2",
      output: cstr: "4",
      input_tokens: array: [cstr: "2", cstr: "+", cstr: "2"],
      output_tokens: array: [cstr: "4"]
    },
    object: {
      input: cstr: "5 * 3",
      output: cstr: "15",
      input_tokens: array: [cstr: "5", cstr: "*", cstr: "3"],
      output_tokens: array: [cstr: "15"]
    },
    object: {
      input: cstr: "10 - 7",
      output: cstr: "3",
      input_tokens: array: [cstr: "10", cstr: "-", cstr: "7"],
      output_tokens: array: [cstr: "3"]
    }
  ],
  tags: array: [cstr: "arithmetic", cstr: "example", cstr: "training"],
  semantic_meta: object: {}  // Empty root object for additional AI annotations
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
