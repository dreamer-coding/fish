# 🐠 Fish Tool
### A Command-Line AI Development Utility by **Fossil Logic**

Fish Tool is a modern, **all-in-one command-line toolkit** designed for developers, researchers, and AI practitioners. It streamlines the creation, training, management, and introspection of **Jellyfish AI modules** and datasets into a single, unified interface — combining traditional UNIX-style commands with **Jellyfish AI integration** for intelligent development workflows.

---

## 🚀 Features

- 🧰 Full AI module lifecycle: create, train, export, import, delete
- 🧠 Integrated **Jellyfish AI** commands (`chat`, `ask`, `summary`) for testing and inference
- 📚 Dataset management and sampling
- 🔍 Module introspection: layers, weights, tokens, activations, and graph visualizations
- 🛡 Auditing tools for safety, bias, and performance evaluation
- 🧩 Cross-platform design (Linux, macOS, Windows)
- 💬 AI-augmented explanations and reporting for modules and datasets

---

## 🧭 Command Palette

---

### 🗂 Core AI & Module Operations

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|-----------------|
| `create` | Create a new AI module, dataset, or project template. | `-t, --type <module/dataset/project>` Component type<br>`-n, --name <id>` Name of component<br>`--template <basic/rlhf/embedding/vision>` Prebuilt template<br>`--from <path>` Initialize from path<br>`--config <file>` Custom configuration |
| `delete` | Remove modules, datasets, or checkpoints. | `-f, --force` Skip confirmation<br>`--module <name>` Delete module<br>`--dataset <name>` Delete dataset<br>`--checkpoint <id>` Remove checkpoint |
| `train` | Train an AI module with a dataset. | `-m, --module <name>` Target module<br>`-d, --dataset <name>` Training dataset<br>`--resume <checkpoint>` Resume training<br>`--epochs <n>` Number of epochs<br>`--steps <n>` Maximum steps<br>`--lr <rate>` Learning rate<br>`--eval` Run evaluation<br>`--save-every <n>` Save checkpoints periodically |
| `audit` | Run performance, bias, and safety audits. | `-m, --module <name>` Module to audit<br>`--bias` Bias detection<br>`--perf` Performance metrics<br>`--safety` Safety checks<br>`--explain` Explain results |
| `introspect` | Inspect module internals (layers, weights, tokens, graph). | `-m, --module <name>` Target module<br>`--layers` Show layer structure<br>`--weights` Weight stats<br>`--tokens` Tokenizer info<br>`--graph` Computation graph<br>`--params` Parameter count<br>`--activations` Activation samples |
| `export` | Export modules, weights, or checkpoints. | `-m, --module <name>` Module to export<br>`-f, --format <onnx/jelly/bin/fson/json>` Export format<br>`--output <path>` Output path |
| `import` | Import modules, datasets, or checkpoints. | `-f, --file <path>` Input file<br>`--type <module/dataset/checkpoint>` Asset type<br>`--name <id>` Assign name |
| `show` | List installed modules, datasets, or checkpoints. | `--modules` Show modules<br>`--datasets` Show datasets<br>`--checkpoints` Show checkpoints<br>`--config` Show default configs<br>`--system` Show system info |
| `view` | Display dataset samples, training metrics, or checkpoint info. | `--dataset <name>` Preview dataset<br>`--samples <n>` Show N samples<br>`--metrics <name>` Show metrics<br>`--checkpoint <id>` Checkpoint details |
| `help` | Display help for commands. | `--examples` Show usage examples<br>`--man` Full manual |

---

### 🤖 AI Commands (Jellyfish Integration)

| **Command** | **Description** | **Common Flags** |
|-------------|-----------------|-----------------|
| `chat` | Start an interactive AI session with a module. | `-m, --module <name>` Module to chat with<br>`--system <file>` System role prompt<br>`--context` Keep session state<br>`--save <file>` Save chat transcript |
| `ask` | Ask a module a one-shot question. | `-m, --module <name>` Target module<br>`--file <path>` Provide context<br>`--analyze` Deep analysis<br>`--explain` Explain output<br>`-q, --quiet` Minimal output |
| `summary` | Generate a concise summary of a file, dataset, or module. | `--file <path>` File summary<br>`--dataset <name>` Dataset summary<br>`-m, --module <name>` Module summary<br>`--depth <level>` Detail level 1–10<br>`--color` Highlight key points<br>`-q, --quiet` Minimal output |

---

### 🌍 Global Flags (Available to All Commands)

| **Flag** | **Description** |
|-----------|-----------------|
| `--help` | Show command help. |
| `--version` | Display Fish Tool version. |
| `-v, --verbose` | Enable detailed output. |
| `-q, --quiet` | Suppress standard output. |
| `--dry-run` | Simulate actions without changes. |
| `--color` | Colorize output where applicable. |
| `--time` | Display timestamps in output. |

---

### 🧩 Usage Examples

| **Example** | **Description** |
|-------------|-----------------|
| `fish create --type module --name jelly-small --template basic` | Create a new AI module with a basic template. |
| `fish train -m jelly-small -d corpus --epochs 3 --save-every 1000` | Train a module with dataset and periodic checkpointing. |
| `fish introspect -m jelly-small --layers --params` | Inspect layer structure and parameter count. |
| `fish audit -m jelly-small --perf --bias --explain` | Run performance, bias, and explanation audit. |
| `fish chat -m jelly-small "Explain this output"` | Start an interactive chat with the module. |
| `fish export -m jelly-small --format onnx --output out/model.onnx` | Export module as ONNX file. |
| `fish show --modules` | List all installed modules. |
| `fish view --dataset news --samples 10` | Preview 10 samples from a dataset. |

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
