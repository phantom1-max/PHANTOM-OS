/*
  PHANTOM OS - ESP32-S3 starter firmware
  Target: ZEVRA ESP32-S3 N16R8 (16 MB flash, 8 MB PSRAM)
  Display: Android phone browser over Wi-Fi
  Storage: microSD integration will be added after confirming board GPIO pinout.

  Build with Arduino IDE + "ESP32 by Espressif Systems" board package.
  Select an ESP32-S3 Dev Module board profile.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <esp_system.h>

const char* AP_SSID = "PHANTOM-OS";
const char* AP_PASSWORD = "phantom123"; // Change this before using outside your home.

WebServer server(80);
unsigned long bootMillis = 0;

const char INDEX_HTML[] PROGMEM = R"PHANTOMHTML(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<meta name="theme-color" content="#050907">
<title>PHANTOM OS</title>
<style>
:root{--bg:#050907;--panel:#0b1410;--panel2:#0e1c15;--line:#1d4933;--green:#54ff9b;--dim:#83b99a;--text:#d9ffe5;--amber:#ffcc66}
*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at 50% -10%,#123522 0,#050907 55%);color:var(--text);font:14px ui-monospace,SFMono-Regular,Menlo,Consolas,monospace;min-height:100vh}
header{display:flex;justify-content:space-between;align-items:center;padding:14px 16px;border-bottom:1px solid var(--line);background:#07100bdd;position:sticky;top:0;z-index:5;backdrop-filter:blur(10px)}
.brand{font-weight:900;letter-spacing:3px;color:var(--green);font-size:17px}.sub{color:var(--dim);font-size:10px;letter-spacing:1.5px;margin-top:4px}.pill{border:1px solid var(--line);border-radius:99px;padding:6px 9px;color:var(--green);font-size:10px}
main{padding:14px;max-width:1000px;margin:auto}.hero{padding:18px;border:1px solid var(--line);background:linear-gradient(135deg,#0d2016dd,#07100fee);border-radius:14px;box-shadow:0 0 30px #00ff6610}
.eyebrow{color:var(--green);font-size:10px;letter-spacing:2px}.hero h1{font-size:clamp(25px,7vw,42px);margin:9px 0 4px;letter-spacing:2px}.hero p{color:var(--dim);line-height:1.5;margin:0}.statusline{display:flex;gap:8px;flex-wrap:wrap;margin-top:15px}
.grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:10px;margin-top:12px}.tile,.window{border:1px solid var(--line);border-radius:12px;background:linear-gradient(145deg,#0c1912,#07100b);padding:13px;min-width:0}.tile{cursor:pointer;text-align:left;color:var(--text);font:inherit;min-height:100px}.tile:active{transform:scale(.985);border-color:var(--green)}.tile .ico{font-size:22px}.tile h3{font-size:13px;margin:10px 0 5px;color:var(--green)}.tile p{font-size:11px;line-height:1.4;color:var(--dim);margin:0}
.window{margin-top:12px;display:none}.window.active{display:block}.winhead{display:flex;justify-content:space-between;align-items:center;border-bottom:1px solid var(--line);padding-bottom:10px;margin-bottom:10px;color:var(--green);font-weight:bold}.close{border:1px solid var(--line);background:#0a1710;color:var(--green);border-radius:7px;padding:5px 9px;font:inherit}
.metrics{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:8px}.metric{padding:10px;background:#06100a;border:1px solid #173a29;border-radius:8px}.metric small{display:block;color:var(--dim);font-size:10px}.metric strong{display:block;color:var(--green);font-size:18px;margin-top:6px;overflow-wrap:anywhere}
#terminalOut{height:190px;overflow:auto;white-space:pre-wrap;background:#030604;border:1px solid #173a29;border-radius:8px;padding:10px;line-height:1.5;color:#aaffc5;font-size:12px}
.cmdrow{display:flex;gap:7px;margin-top:8px}.cmdrow input{min-width:0;flex:1;background:#030604;color:var(--green);border:1px solid var(--line);border-radius:8px;padding:11px;font:inherit}.cmdrow button,.action{background:#10331f;border:1px solid #287449;color:var(--green);border-radius:8px;padding:10px;font:inherit}
.listrow{padding:10px 0;border-bottom:1px solid #173a29;color:var(--dim)}.listrow b{color:var(--text)}footer{text-align:center;color:#527d62;font-size:10px;padding:22px 8px;letter-spacing:1px}
@media(min-width:650px){.grid{grid-template-columns:repeat(4,minmax(0,1fr))}.metrics{grid-template-columns:repeat(4,minmax(0,1fr))}.hero{padding:24px}}
</style>
</head>
<body>
<header><div><div class="brand">PHANTOM<span style="color:#d9ffe5">/OS</span></div><div class="sub">POCKET CYBERDECK // BUILD 0.1</div></div><div class="pill" id="netPill">● CONNECTING</div></header>
<main>
<section class="hero"><div class="eyebrow">LOCAL SYSTEM // ESP32-S3</div><h1>WELCOME BACK.</h1><p>A tiny computer. Your rules. This is the first boot interface; features will expand as hardware is tested.</p><div class="statusline"><span class="pill" id="uptimePill">UPTIME --</span><span class="pill" id="heapPill">HEAP --</span><span class="pill">MODE: CYBERDECK</span></div></section>
<section class="grid">
<button class="tile" onclick="openWin('terminal')"><div class="ico">›_</div><h3>TERMINAL</h3><p>Run system and diagnostic commands.</p></button>
<button class="tile" onclick="openWin('monitor')"><div class="ico">▥</div><h3>SYSTEM MONITOR</h3><p>Live uptime, memory, and network state.</p></button>
<button class="tile" onclick="openWin('files')"><div class="ico">▤</div><h3>FILE EXPLORER</h3><p>Storage panel; microSD support comes next.</p></button>
<button class="tile" onclick="openWin('settings')"><div class="ico">⚙</div><h3>SETTINGS</h3><p>Network details and project information.</p></button>
</section>
<section class="window" id="terminal"><div class="winhead"><span>TERMINAL // LOCAL CONSOLE</span><button class="close" onclick="closeWins()">CLOSE</button></div><div id="terminalOut">PHANTOM OS [build 0.1]
Type 'help' to list commands.
</div><form class="cmdrow" id="cmdForm"><input id="cmdInput" autocomplete="off" placeholder="phantom@esp32:~$ help"><button type="submit">RUN</button></form></section>
<section class="window" id="monitor"><div class="winhead"><span>SYSTEM MONITOR</span><button class="close" onclick="closeWins()">CLOSE</button></div><div class="metrics"><div class="metric"><small>UPTIME</small><strong id="mUptime">--</strong></div><div class="metric"><small>FREE HEAP</small><strong id="mHeap">--</strong></div><div class="metric"><small>CHIP</small><strong id="mChip">--</strong></div><div class="metric"><small>WIFI CLIENTS</small><strong id="mClients">--</strong></div></div></section>
<section class="window" id="files"><div class="winhead"><span>FILE EXPLORER</span><button class="close" onclick="closeWins()">CLOSE</button></div><div class="listrow"><b>/</b> &nbsp; PHANTOM root</div><div class="listrow">▸ <b>/system</b> &nbsp; firmware and interface (embedded)</div><div class="listrow">▸ <b>/storage</b> &nbsp; microSD not configured yet</div><p style="color:var(--dim);font-size:11px">This is a placeholder view. Real file listing and read/write will be enabled after confirming the board GPIO pinout and SD wiring.</p></section>
<section class="window" id="settings"><div class="winhead"><span>SETTINGS</span><button class="close" onclick="closeWins()">CLOSE</button></div><div class="listrow"><b>Device AP:</b> PHANTOM-OS</div><div class="listrow"><b>Address:</b> 192.168.4.1</div><div class="listrow"><b>Build:</b> 0.1 starter</div><div class="listrow"><b>Input:</b> touch via phone browser; keyboard support depends on connection method</div><p style="color:var(--dim);font-size:11px">Change the Wi-Fi password in the firmware before using this beyond a private prototype.</p></section>
</main><footer>PHANTOM OS // EXPERIMENTAL FIRMWARE // NOT A GENERAL-PURPOSE PC OS</footer>
<script>
const $=id=>document.getElementById(id);
function openWin(id){document.querySelectorAll('.window').forEach(w=>w.classList.remove('active'));$(id).classList.add('active');$(id).scrollIntoView({behavior:'smooth',block:'nearest'});if(id==='terminal')$('cmdInput').focus()}
function closeWins(){document.querySelectorAll('.window').forEach(w=>w.classList.remove('active'))}
function fmt(ms){let s=Math.floor(ms/1000),d=Math.floor(s/86400);s%=86400;let h=Math.floor(s/3600);s%=3600;let m=Math.floor(s/60);s%=60;return (d?d+'d ':'')+String(h).padStart(2,'0')+':'+String(m).padStart(2,'0')+':'+String(s).padStart(2,'0')}
async function refresh(){try{let r=await fetch('/api/status');let s=await r.json();$('netPill').textContent='● ONLINE';$('uptimePill').textContent='UPTIME '+fmt(s.uptime_ms);$('heapPill').textContent='HEAP '+Math.round(s.free_heap/1024)+' KB';$('mUptime').textContent=fmt(s.uptime_ms);$('mHeap').textContent=Math.round(s.free_heap/1024)+' KB';$('mChip').textContent=s.chip+' / '+s.cores+'C';$('mClients').textContent=s.clients}catch(e){$('netPill').textContent='● OFFLINE'}}
async function runCmd(cmd){let out=$('terminalOut');out.textContent+='\nphantom@esp32:~$ '+cmd+'\n';try{let r=await fetch('/api/cmd?c='+encodeURIComponent(cmd));let j=await r.json();out.textContent+=j.output+'\n'}catch(e){out.textContent+='Error: device not reachable.\n'}out.scrollTop=out.scrollHeight}
$('cmdForm').addEventListener('submit',e=>{e.preventDefault();let v=$('cmdInput').value.trim();if(v){runCmd(v);$('cmdInput').value=''}});
refresh();setInterval(refresh,2500);
</script>
</body></html>
)PHANTOMHTML";

String jsonEscape(const String& in) {
  String out;
  for (size_t i = 0; i < in.length(); i++) {
    char c = in[i];
    if (c == '"' || c == '\\') out += '\\';
    if (c == '\n') { out += "\\n"; continue; }
    if (c == '\r') continue;
    out += c;
  }
  return out;
}

void handleStatus() {
  String json = "{";
  json += "\"uptime_ms\":" + String(millis() - bootMillis) + ",";
  json += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
  json += "\"chip\":\"ESP32-S3\",";
  json += "\"cores\":" + String(ESP.getChipCores()) + ",";
  json += "\"clients\":" + String(WiFi.softAPgetStationNum());
  json += "}";
  server.send(200, "application/json", json);
}

void handleCommand() {
  String cmd = server.arg("c");
  cmd.trim();
  String output;
  if (cmd == "help") {
    output = "Available commands:\\n  help   - show commands\\n  status - show device status\\n  memory - show free heap\\n  uptime - show uptime\\n  wifi   - show access point info\\n  clear  - clear terminal display (browser-side)";
  } else if (cmd == "status") {
    output = "PHANTOM OS build 0.1\\nChip: ESP32-S3\\nCores: " + String(ESP.getChipCores()) +
             "\\nFree heap: " + String(ESP.getFreeHeap()) + " bytes\\nFlash: " +
             String(ESP.getFlashChipSize() / (1024 * 1024)) + " MB reported";
  } else if (cmd == "memory") {
    output = "Free heap: " + String(ESP.getFreeHeap()) + " bytes\\nLargest free block: " + String(ESP.getMaxAllocHeap()) + " bytes";
  } else if (cmd == "uptime") {
    output = "Uptime: " + String((millis() - bootMillis) / 1000) + " seconds";
  } else if (cmd == "wifi") {
    output = "SSID: " + String(AP_SSID) + "\\nAddress: " + WiFi.softAPIP().toString() +
             "\\nConnected clients: " + String(WiFi.softAPgetStationNum());
  } else if (cmd == "clear") {
    output = "Terminal output cannot be cleared by device command yet; refresh the page to reset it.";
  } else {
    output = "Unknown command: " + cmd + "\\nType 'help' for available commands.";
  }
  server.send(200, "application/json", "{\"output\":\"" + jsonEscape(output) + "\"}");
}

void setup() {
  bootMillis = millis();
  Serial.begin(115200);
  delay(250);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  Serial.println();
  Serial.println("PHANTOM OS starting...");
  Serial.print("Connect phone to Wi-Fi: "); Serial.println(AP_SSID);
  Serial.print("Password: "); Serial.println(AP_PASSWORD);
  Serial.print("Open browser at: "); Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
  });
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/cmd", HTTP_GET, handleCommand);
  server.onNotFound([]() { server.send(404, "text/plain", "PHANTOM OS: endpoint not found"); });
  server.begin();
}

void loop() {
  server.handleClient();
}
