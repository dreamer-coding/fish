# 🐟 Fish Tool
### A Command-Line Utility for **Jellyfish AI Development, Training, and Management**

Fish is a unified CLI tool to **develop, train, analyze, and deploy Jellyfish AI models**, with full support for datasets and classic Fossil Logic AI operations.

---

## 🚀 Features

- 🧠 Manage AI models: create, train, test, deploy, and inspect.
- 🗂 Full dataset lifecycle management: import, preprocess, clean, augment, and export.
- 💬 Classic AI commands: chat, ask, summarize.
- 📊 Monitor training, metrics, and model state.
- 🔄 Cross-platform and modular design.

---

## 🧭 Command Palette

---

### ⚙️ General (AI Model Operations)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|-----------------|
| `create` | Initialize a new Jellyfish AI model. | `-n, --name <name>` Model name<br>`--type <type>` Model architecture<br> |
| `train` | Train a model on a dataset. | `-d, --dataset <path>` Dataset to train on<br>`--epochs <n>` Number of epochs<br>`--batch <n>` Batch size<br>`--lr <rate>` Learning rate |
| `test` | Evaluate model performance. | `-d, --dataset <path>` Test dataset<br>`--metrics <list>` Metrics to report<br>`--save <file>` Save report |
| `inspect` | Inspect model parameters, weights, or configuration. | `--weights` Show weights<br>`--summary` Architecture summary<br>`--layer <name>` Specific layer info |
| `save` | Save current model state. | `--file <path>` Target file<br>`--format <f>` Format: fson, json, bin |
| `load` | Load an existing model. | `--file <path>` Source file<br>`--override` Replace current session |
| `dataset` | Manage datasets for model training and evaluation. | `import`, `clean`, `preprocess`, `augment`, `export`, `stats`, `split` subcommands<br>See Dataset section below for flags |

---

### 🗂 Dataset (Full Dataset Lifecycle)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|-----------------|
| `dataset import` | Import dataset from local or remote source. | `--file <path>` File path<br>`--format <f>` Format: csv, fson, json |
| `dataset clean` | Clean dataset (remove nulls, duplicates, errors). | `--drop-null` Drop null rows<br>`--dedup` Remove duplicates<br>`--normalize` Normalize values |
| `dataset preprocess` | Prepare dataset for training. | `--tokenize` Tokenize text<br>`--scale` Scale numeric data<br>`--encode` Encode categorical data |
| `dataset augment` | Perform data augmentation. | `--type <type>` Augmentation type: noise, flip, shift<br>`--factor <n>` Multiplication factor |
| `dataset export` | Export dataset to file or format. | `--file <path>` Target file<br>`--format <f>` Format: csv, fson, json |
| `dataset stats` | Show dataset statistics. | `--summary` High-level stats<br>`--columns <list>` Specific columns<br>`--plot` Generate plots |
| `dataset split` | Split dataset for training/testing/validation. | `--train <%>` Train fraction<br>`--val <%>` Validation fraction<br>`--test <%>` Test fraction |

---

### 🤖 AI (Classic Fossil Logic AI Commands)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|-----------------|
| `ask` | Run a one-shot prompt against a model. | `-m, --model <id>` Model to use<br>`-f, --file <path>` Provide file context<br>`--explain` Request explanation |
| `chat` | Interactive conversation session with a model. | `--context` Keep conversation history<br>`--save <file>` Save chat transcript<br>`-m, --model <id>` Model to use |
| `summary` | Summarize datasets, chains, logs, or model states. | `-f, --file <path>` File to summarize<br>`--depth <n>` Summary depth<br>`--time` Show timestamps |

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

### ⚙️ General (AI Model Operations)

| **Example** | **Description** |
|-------------|-----------------|
| `fish create --name classifier --type module` | Create a new Jellyfish AI model named "classifier" of type "module". |
| `fish train --dataset data/train --epochs 5 --batch 32 --lr 0.001` | Train a model on a dataset with specific epochs, batch size, and learning rate. |
| `fish test --dataset data/test --metrics accuracy,loss --save results.json` | Test model performance and save the report. |
| `fish inspect --weights --summary --layer conv1` | Inspect model weights, summary, and details for a specific layer. |
| `fish save --file out/model.fson --format fson` | Save the current model state in FSON format. |
| `fish load --file out/model.fson --override` | Load a model from file, replacing the current session. |
| `fish dataset import --file data.csv --format csv` | Import a dataset from a CSV file. |
| `fish dataset clean --drop-null --dedup --normalize` | Clean dataset by dropping nulls, removing duplicates, and normalizing values. |
| `fish dataset preprocess --tokenize --scale --encode` | Preprocess dataset with tokenization, scaling, and encoding. |
| `fish dataset augment --type noise --factor 2` | Augment dataset by adding noise with a multiplication factor. |
| `fish dataset export --file out/data.fson --format fson` | Export dataset to FSON format. |
| `fish dataset stats --summary --columns age,salary --plot` | Show summary stats for specific columns and generate plots. |
| `fish dataset split --train 70 --val 15 --test 15` | Split dataset into train, validation, and test sets. |
| `fish ask --model classifier "Explain this error code"` | Run a one-shot prompt against the "classifier" model. |
| `fish chat --model classifier --context --save chat.log` | Start an interactive chat session and save the transcript. |
| `fish summary --file data.fson --depth 3 --time` | Summarize a file with depth and timestamps. |

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
    fish --help
    ```

## **Contributing**

Interested in contributing? Please open pull requests or create issues on the [GitHub repository](https://github.com/fossillogic/fish).

## **Feedback and Support**

For issues, questions, or feedback, open an issue on the [GitHub repository](https://github.com/fossillogic/fish/issues).

## **License**

This project is licensed under the [Apache 2.0 License](LICENSE).
