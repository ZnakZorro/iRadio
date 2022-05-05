console.log("/www/radio.js");
    const $=e=>document.querySelector(e);
    const $$=e=>document.querySelectorAll(e);
    const ileNumerow = 6;

    let streamsDEF = [
    {"a":"0", "n": "TOK-FM","s":"http://pl-play.adtonos.com/tok-fm"},
    {"a":"0", "n": "RNS",   "s":"http://stream.rcs.revma.com/ypqt40u0x1zuv"},
    {"a":"0", "n": "357",   "s":"http://stream.rcs.revma.com/ye5kghkgcm0uv"}, 
    {"a":"2", "n": "SwissJazz","s":"http://stream.srg-ssr.ch/m/rsj/mp3_128"},
    {"a":"2", "n": "SwissClassic","s":"http://stream.srg-ssr.ch/m/rsc_de/mp3_128"},
    {"a":"2", "n": "ClassicFM", "s":"http://media-the.musicradio.com/ClassicFM"},
    
    {"a":"1", "n": "RMF Classic","s":"http://www.rmfon.pl/n/rmfclassic.pls"},
    {"a":"1", "n": "Dwojka","s":"http://mp3.polskieradio.pl:8952/;"},
    //{"a":"1", "n": "RockService","s":"http://stream.open.fm/379"},
    {"a":"1", "n": "101 Jazz","s":"https://ais-sa2.cdnstream1.com/b22139_128mp3"},
    {"a":"0", "n": "Szczecin","s":"http://stream4.nadaje.com:11986/prs.aac.m3u"},
    //{"a":"0", "n": "Anty-Radio","s":"http://www.emsoft.ct8.pl/inne/anty.m3u"},
    //{"a":"0", "n": "Campus","s":"http://193.0.98.66:8002/listen.pls"},
    //{"a":"0", "n": "PR24","s":"http://stream3.polskieradio.pl:8480/listen.pls"},
    //{"a":"0", "n": "Szczecin Plus","s":"http://waw01-01.ic.smcdn.pl:8000/4260-1.aac.m3u"},
    //{"a":"0", "n": "Szczecin Eska","s":"http://waw01-01.ic.smcdn.pl:8000/2260-1.aac.m3u"},
    ];
    console.log(streamsDEF);
      
    let urlDEF = "/";  
    let urlObj = new URL(window.location.href);
    urlDEF = urlObj.searchParams.get("url") || urlDEF;
    console.log("urlDEF=",urlDEF);
    localStorage.setItem("urlDEF", urlDEF); 
    let url = urlDEF;
     
    let ClearStorage=()=>{
      localStorage.removeItem("streamsDEF");	
    }
  
    const opisInfoRadio=(tx)=>{
      $("#inforadio").textContent  = tx;  
      let arr = tx.split("!");
      console.log(arr);
      let sta = arr[0] || "1";
      let vol = arr[1] || "1";
      $("#sta").textContent  = parseInt(sta)+1;
      $("#vol").textContent  = vol; 
      $("#rsid").textContent = arr[2] || "-0";
      $("#name").textContent = arr[3] || "-0";
      $("#info").textContent = arr[4] || "info";   
      let eq = arr[5] || "0,0,0";
      let ea = eq.split(",");
      //console.log(ea);
          // sliders
          $("#v").value  = vol
          $("#labv").textContent  = vol+"dB";  
          $("#eqlow").value  = ea[0];    $("#labeqlow").textContent  = ea[0]+"dB";  
          $("#eqmid").value  = ea[1];    $("#labeqmid").textContent  = ea[1]+"dB";  
          $("#eqhig").value  = ea[2];    $("#labeqhig").textContent  = ea[2]+"dB";  
          
          let czas = arr[6] || "60";
          $$(".sleep button").forEach(s=>{
            //console.log(czas,s,czas===s.dataset.time);
            if (czas===s.dataset.time) s.classList.add("active");
          })
    
          let host = arr[7] || null;
          removeActive("div.tone button")
          $$(".tone button")[parseInt(eq)].classList.add("active");
      removeActive("div.radia button")
      $("#st"+sta).classList.add("active");  
      if (host){
        $("#stream").textContent = host;
        //console.log("====================host=",host);
        // find host in streamsDEF
        streamsDEF.forEach((a,nr)=>{
          if (a.s===host){
              let data = streamsDEF[nr];
              $("#st"+nr).classList.add("active");
              $("#sta").textContent  = nr;
              $("#name").textContent = data.n;
              $("#stream").textContent = data.stream;
          }
        });
      }  
      
    }
  
    const removeActive=(dom)=>{
      $$(dom).forEach((e)=>e.classList.remove("active"));
    }

    const radioByNumer=(el)=>{
        let nr = parseInt(el.target.dataset.st);
        radioSlij(el,nr)
    }

    const radioSlij=(el,nr=null)=>{
      //console.log("#77 radioSlij",nr,isNaN(nr),!isNaN(nr), (nr != null && isNaN(nr)));
      //console.log(el,nr);
        removeActive("div.radia button");
        el.target.classList.add("active");
        let data = el.target.dataset;
        $("#name").textContent = data.n;
        $("#stream").textContent = data.stream;
        $("#newName").value=data.n;
        $("#newURL").value=data.stream;
        if (nr === null) radioFETCH("x="+data.stream);
        else             radioFETCH("s="+nr);
        //refreshInfo(3);
        refreshInfo(6);
        let sta = parseInt(el.target.dataset.st);
        $("#sta").textContent = (sta+1).toString();
        
    }
  

    const refreshInfo=(sek=5)=> {
       setTimeout(()=>{radioFETCH("n=0");},1000*sek);
    }
    
    const checkForError = response => {
      if (!response.ok) throw Error(response.statusText);
      return response.text();
    };
    const radioFETCH=(param) =>{
        let u = url+"radio?"+param; 
        fetch(u)
        .then(checkForError)
        //.then(r => {return r.text()})
        .then(tx => {
            opisInfoRadio(tx);  
        })
        .catch(e => {console.log(e)})
    }
    const paramFETCH=(param) =>{
      let u = url+"key?"+param; 
      fetch(u)
      .then(checkForError)
      .then(tx => {
          opisInfoRadio(tx);  
      })
      .catch(e => {console.log(e)})
  }  
  const odczytajStacje=()=>{
      //console.log("zapiszStacje")
      //console.log(streamsDEF);
      if (localStorage.getItem("streamsDEF")){ 
        let odczyt = localStorage.getItem("streamsDEF");
        //console.log(odczyt)   
        return JSON.parse(odczyt);
      } else return null;   
  }

  const zapiszStacje=()=>{
        //console.log("zapiszStacje")
        //console.log(streamsDEF)
        localStorage.setItem("streamsDEF", JSON.stringify(streamsDEF));
  }

  const czyreboot=()=>{
    if (confirm("Czy reboot?") == true) {
      radioFETCH("r=0");
    }
  }
  const czysleep=()=>{
    if (confirm("Czy SLEEP?") == true) {
      radioFETCH("e=0");
    }
  }
    const UsunAtualna=()=>{
      if (confirm("Usunąć aktywną stację") == true) {
      //console.log(streamsDEF);
      let active = document.querySelector("div.radia button.active");
      console.log("active=",active);
      if (active){
          let nrst = active.dataset.st;
          console.log("nrst=",nrst);
          console.log(streamsDEF[nrst]);
          if (nrst && parseInt(nrst)>5) {
            delete streamsDEF[nrst];
            streamsDEF = streamsDEF.filter(Object);
            zapiszStacje();
            wpiszStacje();
          }
      }
      //console.log(active);
    } 
    
  }
  
    const wpiszStacje=()=>{  
        //console.log("#wpiszStacje");
        //console.log(streamsDEF,streamsDEF.length)
        if (!streamsDEF) {alert("Brak stacji"); return;}
        let html = "";
        
        streamsDEF.forEach((s,i)=>{
          //console.log(i,i<ileNumerow)
          let klasa = 'class="info"';
          if (i<6) klasa = 'class="info nr"';
          html += '<button '+klasa+' id="st'+i+'" data-st="'+i+'" data-n="'+s.n+'" data-a="'+s.a+'" data-stream="'+s.s+'" >'+s.n+'</button>';
        });
        document.querySelector("div.radia").innerHTML = html;
        document.querySelectorAll("div.radia button").forEach((e,i)=>{
            if (i<6) e.addEventListener('click', radioByNumer, false)
            else     e.addEventListener('click', radioSlij, false)
        }); 
        
    }
  
    
  
    const sendURL=(t)=>{
       let newName = $("#newName").value;
       let newURL = $("#newURL").value;
      //console.log(newName,newURL);
      if(newName && newURL){
          streamsDEF.push({"a":"0", "n": newName,"s":newURL});
          $("#name").textContent = newName;
          zapiszStacje();
      }
      if(newURL) radioFETCH("x="+newURL);
      refreshInfo(3);
      wpiszStacje();
    }
    
    const setTone=(t)=>{
        //console.log(t.dataset.tone);
        radioFETCH("q="+t.dataset.tone);
        removeActive("div.tone button")
        t.classList.add("active");
    }
    const goSleep=(t)=>{
        //console.log(t.dataset.time);
        radioFETCH("y="+t.dataset.time);
        removeActive("div.sleep button")
        t.classList.add("active");
    }
    
    const zapiszDane=()=>{
        //let json = JSON.stringify(streamsDEF);
        //radioFETCH("z="+json);
        //refreshInfo(3);
        let EOL="\r\n";
        let txt="";
        streamsDEF.forEach((v,i)=>{
          if (i>5) return;
          txt += v.n+EOL;
          txt += v.s+EOL;
          paramFETCH("name="+v.n+"&val="+v.s+"&nr="+i);
        })
      console.log(txt)
      radioFETCH("z="+txt);
      refreshInfo(3);
    }
    
    const dyskZapisz=(t)=>{
        //console.log("dyskZapisz");
        let name = "tunnelRadio";
        let jsonTXT = JSON.stringify(streamsDEF,null,"\t"); 
        let blob = new Blob([jsonTXT], {type: "text/plain;charset=utf-8"});
        saveAs(blob, name+".json");   
      }
    
      const readSingleFile=(e)=>{
        //console.log(e)
        //console.log(e.target.files)
        let file = e.target.files[0];
        if (!file) {
          return;
        }
        let reader = new FileReader();
        reader.onload = function(e) {
          let contents = e.target.result;
          if (contents){
            //console.log(contents);
            streamsDEF = JSON.parse(contents);
            wpiszStacje();
            zapiszStacje();
          }
          
        };
        reader.readAsText(file);
      }

    const readStationsJSON=()=>{
      let u="https://znakzorro.github.io/radioTunel/public/json/cartoon.json";
      fetch(u)
        //.then(checkForError)
        .then(r => {return r.json()})
        .then(obj => {
            //console.log(obj.radio);
            //console.log(obj.ampli);
        })
        .catch(e => {console.log(e)})      
    }
      
    // dom ready
      document.addEventListener("DOMContentLoaded",function(){
        readStationsJSON();
        //console.log(odczyt)
        let odczyt=odczytajStacje();
        //console.log(odczyt)
        if(odczyt) streamsDEF = odczyt;
        wpiszStacje();
           
        refreshInfo(1);
        setInterval(()=>{radioFETCH("n=0");},15000);
        $("#title").textContent = document.title;
        document.getElementById('file-input').addEventListener('change', readSingleFile, false);
    });
    // domready