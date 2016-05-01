#include <leveldb/db.h>

extern "C" {
#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/intext.h>
#include <caml/threads.h>
}

leveldb::DB* db;
leveldb::Options options;

extern "C" value ldbopen(value _f) {
  CAMLparam1(_f);
  char* f = String_val(_f);
  options.create_if_missing = true;
  leveldb::Status s = leveldb::DB::Open(options, f, &db);
  if (!s.ok()) caml_failwith(s.ToString().c_str());
}

extern "C" value ldbclose() {
  delete db;
}

extern "C" value ldbput(value _k,value _v) {
  CAMLparam2(_k,_v);
  char* k = String_val(_k);
  char* v = String_val(_v);
  std::string key(k,caml_string_length(_k)); // in case of null characters
  std::string val(v,caml_string_length(_v)); // in case of null characters
  leveldb::Status s = db->Put(leveldb::WriteOptions(), key, val);
  if (!s.ok()) caml_failwith(s.ToString().c_str());
}

extern "C" value ldbget(value _k,value _buf) {
  CAMLparam2(_k,_buf);
  char* k = String_val(_k);
  std::string key(k,caml_string_length(_k)); // in case of null characters
  std::string value;
  leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);
  if (!s.ok()) caml_failwith(s.ToString().c_str());
  int l = caml_string_length(_buf);
  int vlen = value.length();
  if (vlen >= l) caml_failwith("value too long");
  memcpy(String_val(_buf), value.c_str(), vlen);
  CAMLreturnT(int,Val_int(vlen));
}

extern "C" value ldbexists(value _k) {
  CAMLparam1(_k);
  char* k = String_val(_k);
  std::string key(k,caml_string_length(_k)); // in case of null characters
  std::string value;
  bool ret = false;
  leveldb::Status s = db->Get(leveldb::ReadOptions(), key, &value);
  if (s.IsNotFound()) CAMLreturnT(int,Val_int(0));
  if (!s.ok()) caml_failwith(s.ToString().c_str());
  CAMLreturnT(int,Val_int(1));
}

extern "C" value ldbdelete(value _k) {
  CAMLparam1(_k);
  char* k = String_val(_k);
  std::string key(k,caml_string_length(_k)); // in case of null characters
  leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);
  if (!s.ok()) caml_failwith(s.ToString().c_str());
}

