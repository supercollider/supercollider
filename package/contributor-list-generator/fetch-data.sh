#!/usr/bin/env bash
#
# Fetch issues and pull-requests for this milestone from the github REST API

mkdir -p github-data

echo "Please enter milestone ID number."
echo "To find the milestone ID, visit the milestone page in your browser and look at the last part of the URL."
read -p "Milestone ID: " milestone

# Download all labels
curl "https://api.github.com/repos/supercollider/supercollider/labels?per_page=100" -o github-data/labels.json
# Download all issues
# Get the milestone id (not the name) from:
# https://github.com/supercollider/supercollider/milestones
curl "https://api.github.com/repos/supercollider/supercollider/issues?per_page=100&page=[1-5]&state=closed&milestone=$milestone" -o "github-data/issues-closed-#1.json"
# Download all pull-requests
curl "https://api.github.com/repos/supercollider/supercollider/pulls?per_page=100&page=[1-2]&state=closed&milestone=$milestone" -o "github-data/pulls-closed-#1.json"
