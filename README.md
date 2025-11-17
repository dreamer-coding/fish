# 🐠 Fish Tool  
### The Jellyfish AI Development & Training Utility by **Fossil Logic**

**Fish Tool** is a specialized command-line utility dedicated to building, training, inspecting, and managing **Jellyfish AI modules and datasets**. It provides powerful model lifecycle commands, dataset workflows, auditing tools, and AI-assisted testing features.  

---

# 🎛 Command Palette Overview

## 🧬 AI Development Commands (Model Lifecycle)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|------------------|
| `create` | Create a new AI module. | `-n, --name <id>` Name of the AI module |
| `delete` | Remove modules, or checkpoints. | `-f, --force` (force deletion)<br>`-i, --interactive` (prompt before delete) |
| `introspect` | Inspect internal structure of a model or dataset. | `--crossref` (check cross references)<br>`--core` (show core module info)<br>`--chains` (inspect memory chains) |
| `train` | Train or fine-tune a AI module. | `--epochs <n>` (number of epochs)<br>`--batch <n>` (batch size)<br>`--lr <rate>` (learning rate)<br>`--resume <checkpoint>` (resume from checkpoint)<br>`--save <path>` (save output path) |
| `audit` | Analyze model behavior for safety, bias, drift, or anomalies. | `--drift` (check for drift)<br>`--bias` (check for bias)<br>`--toxic` (check for toxicity)<br>`--woke` (check for woke signals)<br>`--explain` (explain findings)<br>`--export <path>` (export results) |
| `imagine` | Generate new content, modules, or data samples. | `-m, --model <id>` (model to use)<br>`-p, --prompt <text>` (prompt text)<br>`--type <fson/json/yaml/etc>` (output type)<br>`--length <n>` (output length)<br>`--count <n>` (number of samples)<br>`--seed <n>` (random seed)<br>`--save <path>` (save output)<br>`--temperature <value>` (sampling temperature) |
| `dataset` | Dataset-focused operations (subcommands). | `add`, `remove`, `tag`, `clean`, `stats`, `split`, `verify` (see Dataset Subcommands below) |
| `prune` | Remove unused or low-value parameters, entries, or metadata. | `--small` (remove small params)<br>`--redundant` (remove redundant items)<br>`--orphans` (remove orphaned entries) |
| `decay` | Apply weight decay or gradual forgetting to models or chains. | `--strength <value>` (decay strength)<br>`--simulate` (simulate decay) |
| `seek` | AI-enhanced search across datasets, chains, or models. | `--query <text>` (search query)<br>`--semantic` (semantic search) |

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
