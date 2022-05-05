
console.log("eq.js");



let changeEQ=(ten,ev)=>{
    console.log(ten);
    console.log(ev);
    let id  = ten.id;
    let val = ten.value;
    console.log(id+"="+val);
    radioFETCH(id+"="+val);
    document.getElementById("lab"+id).innerHTML = val+"dB";
}

let appendEQ=(name="frq")=>{
    let celEQ = document.querySelector(".acordion");
    celEQ.innerHTML += `
    <div class="eqrange">
        <span id="labeq${name}" for="eq${name}">${name}</span>
        <input onClick="changeEQ(this)" value="0" type="range" id="eq${name}" min="-24" max="6" />        
    </div>
    `;
    document.getElementById("eq"+name).addEventListener('change', changeEQ, false);
    /*return
    let elem = document.createElement('input');    
    elem.type="range";
    elem.id="eq"+name;
    elem.min="-6";
    elem.max="24";
    elem.value="0";
    elem.addEventListener('change', changeEQ, false);
    //elem.style.cssText = 'position:absolute;width:100%;height:100%;opacity:0.3;z-index:100;background:#000';
    
    celEQ.appendChild(elem);
    //document.body.appendChild(elem);*/
}

let installEQ=()=>{
    console.log("installEQ");
    //celEQ.innerHTML += "<p>EQ</p>";
    //document.getElementById('file-input').addEventListener('change', readSingleFile, false);
    appendEQ("low");
    appendEQ("mid");
    appendEQ("hig");
}

// dom ready
document.addEventListener("DOMContentLoaded",function(){
    installEQ();
    
    
});
// domready