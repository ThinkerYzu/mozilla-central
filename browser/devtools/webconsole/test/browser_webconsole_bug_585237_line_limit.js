/* vim:set ts=2 sw=2 sts=2 et: */
/* ***** BEGIN LICENSE BLOCK *****
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * Contributor(s):
 *  Patrick Walton <pcwalton@mozilla.com>
 *  Mihai Șucan <mihai.sucan@gmail.com>
 *
 * ***** END LICENSE BLOCK ***** */

// Tests that the Web Console limits the number of lines displayed according to
// the user's preferences.

const TEST_URI = "http://example.com/browser/browser/devtools/webconsole/test//test-console.html";

function test() {
  addTab(TEST_URI);
  browser.addEventListener("DOMContentLoaded", testLineLimit,
                                            false);
}

function testLineLimit() {
  browser.removeEventListener("DOMContentLoaded",testLineLimit, false);

  openConsole();

  let console = browser.contentWindow.wrappedJSObject.console;
  outputNode = HUDService.getHudByWindow(content).outputNode;

  let prefBranch = Services.prefs.getBranch("devtools.hud.loglimit.");
  prefBranch.setIntPref("console", 20);

  for (let i = 0; i < 20; i++) {
    console.log("foo #" + i); // must change message to prevent repeats
  }

  is(countMessageNodes(), 20, "there are 20 message nodes in the output " +
     "when the log limit is set to 20");

  console.log("bar");
  is(countMessageNodes(), 20, "there are still 20 message nodes in the " +
     "output when adding one more");

  prefBranch.setIntPref("console", 30);
  for (let i = 0; i < 20; i++) {
    console.log("boo #" + i); // must change message to prevent repeats
  }

  is(countMessageNodes(), 30, "there are 30 message nodes in the output " +
     "when the log limit is set to 30");

  prefBranch.setIntPref("console", 0);
  console.log("baz");
  is(countMessageNodes(), 0, "there are no message nodes in the output when " +
     "the log limit is set to zero");

  prefBranch.clearUserPref("console");
  prefBranch = console = outputNode = null;
  finishTest();
}

function countMessageNodes() {
  return outputNode.querySelectorAll(".hud-msg-node").length;
}

