open Ser
open Hash

(*** ffi with leveldb ***)

external dbopen : string -> unit = "ldbopen"
external dbclose : unit -> unit = "ldbclose"
external edbput : string -> string -> unit = "ldbput"
external edbget : string -> string -> int = "ldbget"
external edbexists : string -> int = "ldbexists"
external edbdelete : string -> unit = "ldbdelete"

let valstr = String.create 1000000

let tpk tp k =
  let buf = Buffer.create 20 in
  Buffer.add_string buf tp;
  Buffer.add_char buf ':';
  let c = seo_hashval seosb k (buf,None) in
  seosbf c;
  Buffer.contents buf

let dbget tp k seival =
  let l =
    try
      edbget (tpk tp k) valstr
    with Failure("NotFound: ") -> raise Not_found
  in
  let (v,_) = seival (valstr,l,None,0,0) in
  v

let dbexists tp k =
  edbexists (tpk tp k) > 0

let dbput tp k v seoval =
  let buf = Buffer.create 1000 in
  let c = seoval v (buf,None) in
  seosbf c;
  edbput (tpk tp k) (Buffer.contents buf)

let dbdelete tp k =
  edbdelete (tpk tp k)
