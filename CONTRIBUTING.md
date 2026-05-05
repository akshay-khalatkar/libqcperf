# Contributing to libqcperf

Hi there!
We're thrilled that you'd like to contribute to this project.
Your help is essential for keeping this project great and for making it better.

## Branching Strategy

In general, contributors should develop on branches based off of `main` and pull requests should be made against `main`.

## Submitting a pull request

1. Please read our [code of conduct](CODE-OF-CONDUCT.md) and [license](LICENSE.txt).
1. [Fork](https://github.com/qualcomm/libqcperf/fork) and clone the repository.

    ```bash
    git clone https://github.com/<username>/libqcperf.git
    ```

1. Create a new branch based on `main`:

    ```bash
    git checkout -b <my-branch-name> main
    ```

1. Create an upstream `remote` to make it easier to keep your branches up-to-date:

    ```bash
    git remote add upstream https://github.com/qualcomm/libqcperf.git
    ```

1. Make your changes, add tests, and make sure the tests still pass.
1. Commit your changes using the [DCO](https://developercertificate.org/). You can attest to the DCO by commiting with the **-s** or **--signoff** options or manually adding the "Signed-off-by":

    ```bash
    git commit -s -m "Really useful commit message"`
    ```

1. After committing your changes on the topic branch, sync it with the upstream branch:

    ```bash
    git pull --rebase upstream main
    ```

1. Push to your fork.

    ```bash
    git push -u origin <my-branch-name>
    ```

    The `-u` is shorthand for `--set-upstream`. This will set up the tracking reference so subsequent runs of `git push` or `git pull` can omit the remote and branch.

1. [Submit a pull request](https://github.com/qualcomm/libqcperf/pulls) from your branch to `main`.
1. Pat yourself on the back and wait for your pull request to be reviewed.

## Security Analysis of Pull Requests

To maintain the security and integrity of this project, all pull requests from external contributors are automatically scanned using [Semgrep](https://github.com/semgrep/semgrep) to detect insecure coding patterns and potential security flaws.

**Static Analysis with Semgrep:**  We use Semgrep to perform lightweight, fast static analysis on every PR. This helps identify risky code patterns and logic flaws early in the development process.

**Contributor Responsibility:** If any issues are flagged, contributors are expected to resolve them before the PR can be merged.

**Continuous Improvement:** Our Semgrep ruleset evolves over time to reflect best practices and emerging security concerns.

By submitting a PR, you agree to participate in this process and help us keep the project secure for everyone.


## Coding Standards

All contributors must follow the project's coding standards to maintain code consistency and quality. For detailed coding guidelines, including:

- Naming conventions (snake_case, PascalCase, UPPER_CASE)
- Type definitions and typedef usage
- File organization and structure
- Documentation requirements (Doxygen)
- Error handling patterns
- Memory management best practices
- Thread safety considerations

Please refer to the **[Coding Guidelines](DEVELOPMENT-GUIDE.md#coding-guidelines)** section in the DEVELOPMENT-GUIDE.md file.

## Development Guidelines

Here are a few things you can do that will increase the likelihood of your pull request to be accepted:

- **Follow the coding standards**: Adhere to the [coding guidelines](DEVELOPMENT-GUIDE.md#coding-guidelines) documented in DEVELOPMENT-GUIDE.md.
- **Format your code**: Always format code using the `.clang-format` configuration before committing.
- **Write tests**: Add appropriate tests for new features or bug fixes.
- **Keep changes focused**: If you want to make multiple independent changes, please consider submitting them as separate pull requests.
- **Write good commit messages**: Follow [these guidelines](https://tbaggery.com/2008/04/19/a-note-about-git-commit-messages.html) for writing clear commit messages.
- **Use DCO sign-off**: All commits must include a "Signed-off-by" line (use `git commit -s`).
- **Discuss major changes**: For large features, architecture changes, or core code modifications, arrange a discussion with other developers first. PR reviews will go much faster when there are no surprises.

## Backend Development

If you're developing a new backend or extending existing backend functionality, please refer to the comprehensive [Backend Development Guide](DEVELOPMENT-GUIDE.md) which includes:

- Project architecture overview
- Backend interface specifications
- Step-by-step guide for adding new backends
- Backend implementation examples
- Testing guidelines for backends
