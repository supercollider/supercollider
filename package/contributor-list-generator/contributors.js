/**
 * Prints contributors list sorted by number of issues and pull requests.
 * Closed issues and successfully merged pull-requests only.
 */
var fs = require('fs');
var _ = require('lodash');
var ib = [];
var pb = [];

for (var i = 0; i < 10; i++) {
  var jsonPath = `./github-data/pulls-closed-${i}.json`;
  if (fs.existsSync(jsonPath)) {
    var d = require(jsonPath);
    pb.push(d);
  }

  var jsonPath2 = `./github-data/issues-closed-${i}.json`;
  if (fs.existsSync(jsonPath2)) {
    var d = require(jsonPath2);
    ib.push(d);
  }
}

// console.log(pb);

var pulls = _.flatten([].concat(pb));
var issues = _.flatten([].concat(ib));

var pullsMap = {};
_.each(pulls, (p) => {
  pullsMap[p.number] = p;
});

function shouldIncludePull(pull) {
  // has been merged
  return pull.merged_at && shouldIncludeIssue(pull);
}

function shouldIncludeIssue(issue) {
  // console.log({state: issue.state, milestone: issue.milestone && issue.milestone.title});
  var status = (issue.state === 'closed');
  if (!status) {
    return false;
  }

  // labels includes 'invalid' or 'wontfix'
  // if number is in pulls and there is no merged at
  var pull = pullsMap[issue.number];
  if (pull) {
    // was never merged
    if (!pull.merged_at) {
      return false;
    }
  }

  return true;
}

function mapIssueData(pull) {
  // console.log(pull.user);
  return {
    user: {
      name: pull.user.login,
      url: pull.user.html_url
    }
  };
}


var issuesData = issues.filter(shouldIncludeIssue).map(mapIssueData);

var users = {};
_.each(issuesData, (pull) => {
  if (users[pull.user.name]) {
    users[pull.user.name].count += 1;
  } else {
    pull.user.count = 1;
    users[pull.user.name] = pull.user;
  }
});

users = Object.keys(users);
users = _.sortBy(users, (user) => user.toLowerCase());

console.log(users.join(", "));
