# 🐠 Fish Tool  
### The Jellyfish AI Development & Training Utility by **Fossil Logic**

**Fish Tool** is a specialized command-line utility dedicated to building, training, inspecting, and managing **Jellyfish AI modules and datasets**. It provides powerful model lifecycle commands, dataset workflows, auditing tools, and AI-assisted testing features.  

---

# 🎛 Command Palette Overview

## 🧬 AI Development Commands (Model Lifecycle)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|------------------|
| `create` | Create a new AI module, dataset, profile, or training plan. | `-n, --name <id>`<br>`--config <file>`<br>`--template <id>` |
| `delete` | Permanently remove modules, datasets, or checkpoints. | `-f, --force`<br>`-i, --interactive`<br>`--prune` Remove orphaned metadata |
| `introspect` | Inspect internal structure of a model or dataset. | `--weights` Show tensor stats<br>`--layers` Architecture<br>`--tokens` Token stats<br>`--verify` Validate integrity |
| `train` | Train or fine-tune a Jellyfish AI module using a dataset. | `-d, --dataset <path>`<br>`--epochs <n>`<br>`--batch <n>`<br>`--lr <rate>`<br>`--resume <checkpoint>`<br>`--save <path>` |
| `audit` | Analyze model behavior for safety, bias, drift, or anomalies. | `--drift`<br>`--bias`<br>`--toxicity`<br>`--explain`<br>`--export <path>` |
| `show` | Display information about modules, datasets, chains, records, or metadata. | `-a, --all`<br>`--stats`<br>`--meta`<br>`--tags` |
| `view` | View raw dataset entries, tokens, or training samples. | `-n, --number <count>`<br>`--sample` Random selection<br>`--shuffle` |
| `export` | Export a model, dataset, or chain. Supports AI-centric formats for interoperability and deployment. | `-f, --format <fson/onnx/tensor>`<br>`--dest <path>`<br>`--compress`<br>`--include-metadata`<br>`--overwrite`<br>`--quiet` |
| `import` | Import a model, dataset, or configuration from supported AI formats. | `--source <path>`<br>`--validate`<br>`--name <id>`<br>`--replace`<br>`--quiet` |
| `imagine` | Generate new content, modules, or data samples using Jellyfish AI. | `-m, --model <id>`<br>`-p, --prompt <text>`<br>`--type <fson/json/yaml/etc>`<br>`--length <n>`<br>`--count <n>`<br>`--seed <n>`<br>`--save <path>`<br>`--temperature <value>` |
| `dataset` | Dataset-focused operations (subcommands). | `add`, `remove`, `tag`, `clean`, `stats`, `split`, `verify` |
| `merge` | Merge datasets, models, or chains. | `-s, --strategy <union/overwrite/append>`<br>`--dry-run` |
| `rebase` | Apply a dataset or config change onto an existing model or chain. | `--config <file>`<br>`--align` |
| `prune` | Remove unused or low-value parameters, entries, or metadata. | `--small`<br>`--redundant`<br>`--orphans` |
| `decay` | Apply weight decay or gradual forgetting to models or chains. | `--strength <value>`<br>`--simulate` |
| `seek` | AI-enhanced search across datasets, chains, or models. | `--query <text>`<br>`--semantic`<br>`--regex` |

---

## 🧪 AI Testing Commands

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|------------------|
| `ask` | Run a one-shot prompt against a module or chain. | `-m, --model <id>`<br>`-f, --file <path>`<br>`--explain`<br>`--quiet` |
| `chat` | Interactive conversation session with a local module. | `--context` Keep history<br>`--save <file>` Save transcript<br>`-m, --model <id>` |
| `summary` | Summarize datasets, chains, logs, or model states. | `-f, --file <path>`<br>`--depth <n>`<br>`--color`<br>`--time` |

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

### 🔗 How it works

- **model.fson**
  - Contains `version`, `model_type`, `chains` (array of paths), and `records` reference.
  - Acts as the central pointer for the model.

- **chains/**  
  - Stores individual learning chains.
  - Each chain is append-only and contains committed knowledge learned by the AI.
  
- **records.fson**  
  - Logs changes, updates, and training events.
  - Time-aware, allowing the AI to "remember" historical actions.

- **stats.fson**  
  - Tracks global statistics like `commit_count`, `token_count`, `chain_count`.
  - Avoids the need to scan all chains for quick metrics.

### 💡 Notes

- This structure enables **persistent AI memory** while keeping chains modular.
- Chains can be individually inspected, exported, or pruned without touching the main model.
- Records ensure auditability and rollback capability.

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
