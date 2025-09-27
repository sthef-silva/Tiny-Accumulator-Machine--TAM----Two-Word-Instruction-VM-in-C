(() => {
  const programEl = document.getElementById('program');
  const pcEl = document.getElementById('pc');
  const accEl = document.getElementById('acc');
  const statusEl = document.getElementById('status');
  const memEl = document.getElementById('mem');
  const traceEl = document.getElementById('trace');
  const resetBtn = document.getElementById('resetBtn');
  const stepBtn = document.getElementById('stepBtn');
  const runBtn = document.getElementById('runBtn');
  const speedEl = document.getElementById('speed');

  let lines = [];
  let pc = 1;         // 1-based line numbers for clarity
  let acc = 0;
  let halted = false;
  const mem = {};     // simple address -> value

  function parse() {
    const raw = programEl.value.split('\n');
    lines = raw.map(s => s.trim()).map(s => {
      if (!s || s.startsWith(';')) return null;
      const [op, val] = s.split(/\s+/);
      return { op: op?.toUpperCase(), val: val !== undefined ? Number(val) : undefined, raw: s };
    });
    // lines array preserves indexes; line n is at index n-1
  }

  function resetState() {
    parse();
    pc = 1; acc = 0; halted = false;
    for (const k of Object.keys(mem)) delete mem[k];
    setStatus('ready');
    render();
    traceEl.textContent = '';
  }

  function setStatus(s){ statusEl.textContent = s; }
  function render() {
    pcEl.textContent = halted ? '—' : pc;
    accEl.textContent = acc;
    memEl.textContent = Object.keys(mem).length
      ? Object.entries(mem).map(([a,v])=>`${a.padStart(3,' ')} : ${v}`).join('\n')
      : '(empty)';
  }

  function stepOnce() {
    if (halted) return;
    const inst = lines[pc-1];
    if (!inst) { // blank/comment/out-of-range
      pc++;
      if (pc > lines.length) { halted = true; setStatus('halt (end)'); }
      render(); return;
    }
    const {op, val} = inst;
    traceEl.textContent += `PC ${pc}: ${inst.raw}\n`;

    switch(op){
      case 'LOAD': acc = Number(val)||0; pc++; break;
      case 'STORE': mem[String(val||0)] = acc; pc++; break;
      case 'ADD': acc += Number(val)||0; pc++; break;
      case 'SUB': acc -= Number(val)||0; pc++; break;
      case 'JMP': pc = Number(val)||1; break;
      case 'JZ': pc = (acc===0) ? (Number(val)||1) : pc+1; break;
      case 'HALT': halted = true; setStatus('halt'); break;
      default: pc++; break; // unknown -> skip
    }

    if (pc < 1 || pc > lines.length+1) { halted = true; setStatus('halt (range)'); }
    render();
  }

  let running = false;
  async function runLoop(){
    running = true;
    setStatus('running');
    while(!halted && running){
      stepOnce();
      await new Promise(r => setTimeout(r, Number(speedEl.value)));
    }
    if (!running && !halted) setStatus('paused');
  }

  resetBtn.onclick = () => { running = false; resetState(); };
  stepBtn.onclick  = () => { running = false; stepOnce(); };
  runBtn.onclick   = () => { if (!running && !halted) runLoop(); };

  programEl.addEventListener('change', resetState);
  resetState();
})();