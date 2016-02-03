Tree = function(lbl, lT, rT){
    this.label=lbl
    this.leftTree= lT
    this.rightTree= rT
};

function isAlphaNumeric(char) {
  var code
    code = char.charCodeAt(0);
    if (!(code > 47 && code < 58) && // numeric (0-9)
        !(code > 64 && code < 91) && // upper alpha (A-Z)
        !(code > 96 && code < 123)) { // lower alpha (a-z)
      return false;
    }
  return true;
}


function tree(str){
    if(!isAlphaNumeric(str.charAt(0))){        //è un albero
        var partial_left=tree(str.substring(1))       //substring dal dopo la parentesi aperta
        
        var partial_right=tree(partial_left[1].substring(1))    //substring da dopo la virgola
        var label="";
        //parto da 1 per saltare la parentesi chiusa
        var i=1
        for(i=1; isAlphaNumeric(partial_right[1].charAt(i)); i++){
            label+=partial_right[1].charAt(i);
        }
        
        return [new Tree(label, partial_left[0], partial_right[0]), partial_right[1].substring(i)]
    }else{                  //è una foglia
        var lbl=""
        var j=0;
        while(isAlphaNumeric(str.charAt(j))){
            lbl+=str.charAt(j)
            j++
        }
        
        return [new Tree(lbl, null, null), str.substring(j)]
    }
}

function getTree(treeString){
    var str=''
    for(i=0; i<treeString.length; i++){
        if(treeString.charAt(i)===' '){
            continue;
        }
        str+=treeString.charAt(i);
    }   
    
    return tree(str)[0]
}


var aTree="(H2,((H4,H5)H3,H6)H7)H1"
getTree(aTree);