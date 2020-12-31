Module['noInitialRun'] = true;

{
  var a = Module['arguments'];
  if (!a) {
    a = [];
    Module['arguments'] = a;
  }
  if (!a.includes('-u')) a.push('-u', '57110'); // default 'udp' port
  if (!a.includes('-D')) a.push('-D', '0');     // do not load synthdefs
  if (!a.includes('-i')) a.push('-i', '0');     // no input channels
  if (!a.includes('-o')) a.push('-o', '2');     // two output channels
}
