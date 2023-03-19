import os

from git import Repo
from utilities import assert_step, use_directory, run

class GitUtilities:
    @staticmethod
    def check_no_uncommitted_changes(repo: Repo) -> None:
        assert_step(not repo.bare, "There are uncommitted changes in git")

        # From https://stackoverflow.com/questions/31959425/how-to-get-staged-files-using-gitpython
        repo_name = GitUtilities.get_repo_name(repo)
        assert_step(len(repo.index.diff(None)) == 0, f"there are un-committed changes to {repo_name}")  # Modified
        assert_step(len(repo.index.diff("HEAD")) == 0, f"there are staged changes to {repo_name}")  # Staged

    @staticmethod
    def check_branch_name(repo: Repo, branch_name: str) -> None:
        repo_name = GitUtilities.get_repo_name(repo)
        assert_step((repo.active_branch.name == branch_name), f"the {repo_name} repo is not on branch {branch_name}")

    @staticmethod
    def get_repo_name(repo: Repo) -> str:
        repo_location = repo.working_dir
        repo_name = os.path.basename(repo_location)
        return repo_name

    @staticmethod
    def reset_and_clean_working_directory(project_dir: str) -> None:
        with use_directory(project_dir):
            # Delete untracked files:
            # - does not delete ignored files
            # - does not delete untracked files in new, untracked directories
            run(["git", "clean", "-f"])

            run(["git", "reset", "--hard"])

    @staticmethod
    def add_everything(directory: str) -> None:
        with use_directory(directory):
            run(["git", "add", "."])

    @staticmethod
    def commit_everything(directory: str, message: str) -> None:
        with use_directory(directory):
            run(["git", "commit", "-m", F"'{message}'"])

    @staticmethod
    def add_and_commit_everything(directory: str, message: str) -> None:
        GitUtilities.add_everything(directory)
        GitUtilities.commit_everything(directory, message)

    @staticmethod
    def push_active_branch_origin(directory: str) -> None:
        repo = Repo(directory)
        branch = repo.active_branch.name
        repo.remote('origin').push(branch)

    @staticmethod
    def pull_active_branch_origin(directory: str) -> None:
        repo = Repo(directory)
        branch = repo.active_branch.name
        repo.remote('origin').pull(branch)
