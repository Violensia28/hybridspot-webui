export function initTabs(){
  const tabs = document.querySelectorAll('nav#tabs button');
  tabs.forEach(btn=>btn.addEventListener('click',()=>{
    tabs.forEach(b=>b.classList.remove('active'));
    btn.classList.add('active');
    const id = btn.dataset.tab;
    document.querySelectorAll('main section').forEach(s=>s.hidden = true);
    document.getElementById('view-'+id).hidden = false;
  }));
}
export function toast(msg){
  const el = document.createElement('div'); el.className='toast'; el.textContent = msg;
  document.body.appendChild(el); setTimeout(()=>el.remove(), 1800);
}
