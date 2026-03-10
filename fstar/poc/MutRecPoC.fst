(**
 * MutRecPoC -- Proof of Concept: mutual recursion with Stack effect in F*.
 *
 * Two mutually recursive Stack-effect functions.
 * walk_list calls process_item; process_item calls walk_list for nested blocks.
 * This models exec_stmts <-> exec_if_block mutual recursion.
 *)
module MutRecPoC

open FStar.HyperStack.ST
open LowStar.Buffer
open LowStar.BufferOps

module B  = LowStar.Buffer
module U8 = FStar.UInt8
module U32 = FStar.UInt32

let rec walk_list
  (buf : B.buffer U8.t)
  (buf_len : U32.t)
  (off : U32.t)
  (gas : U32.t)
  : Stack unit
      (requires fun h -> B.live h buf /\ B.length buf = U32.v buf_len)
      (ensures  fun h0 _ h1 -> h0 == h1)
      (decreases (U32.v gas))
  =
  if gas = 0ul then ()
  else if U32.gte off buf_len then ()
  else
    let consumed = process_item buf buf_len off (U32.sub gas 1ul) in
    if U32.gt consumed 0ul && U32.lte consumed (U32.sub buf_len off) then
      walk_list buf buf_len (U32.add off consumed) (U32.sub gas 1ul)
    else ()

and process_item
  (buf : B.buffer U8.t)
  (buf_len : U32.t)
  (off : U32.t)
  (gas : U32.t)
  : Stack U32.t
      (requires fun h -> B.live h buf /\ B.length buf = U32.v buf_len /\
                          U32.v off < U32.v buf_len)
      (ensures  fun h0 _ h1 -> h0 == h1)
      (decreases (U32.v gas))
  =
  if gas = 0ul then 1ul
  else
    let tag = buf.(off) in
    if tag = 0x80uy then begin
      (* nested block: recurse into walk_list *)
      if U32.lt (U32.add off 1ul) buf_len then begin
        walk_list buf buf_len (U32.add off 1ul) (U32.sub gas 1ul);
        2ul
      end else 1ul
    end else
      1ul   (* leaf: consume 1 byte *)
