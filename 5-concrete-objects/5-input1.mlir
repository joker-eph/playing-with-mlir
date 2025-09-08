
func.func private @foo()

func.func @bar(%arg0 : f32, %arg1 : f32) {
// CHECK: arith.addf %arg0, %arg1 fastmath<reassoc> : f32
  %add = arith.addf %arg0, %arg1 : f32
  // CHECK: %1 = arith.subf %arg1, %arg0 : f32
  %sub = arith.subf %arg0, %arg1 : f32 
  // CHECK: arith.mulf %arg0, %arg1 : f32 
  %div = arith.divf %arg0, %arg1 : f32  
  return
}
