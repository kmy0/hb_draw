---@meta

---@class hb_draw
---@field cylinder fun(start: Vector3f, end: Vector3f, radius: number, color: integer, outline: boolean, color_outline: integer)
---@field sphere fun(center: Vector3f, radius: number, color: integer, outline: boolean, color_outline: integer)
---@field box fun(pos: Vector3f, extent: Vector3f, rot: Matrix4x4f, color: integer, outline: boolean, color_outline: integer)
---@field triangle fun(pos: Vector3f, extent: Vector3f, rot: Matrix4x4f, color: integer, outline: boolean, color_outline: integer)
---@field capsule fun(start: Vector3f, end: Vector3f, radius: number, color: integer, outline: boolean, color_outline: integer)
---@field sliced_cylinder fun(start: Vector3f, end: Vector3f, radius: number, direction: Vector3f, degrees: number, color: integer, outline: boolean, color_outline: integer)
---@field set_num_segments fun(num: integer)
---@field set_outline_tickness fun(num: integer)
---@field register fun(draw_fn: fun())
--
---@field write_byte fun(ptr: integer, value: integer): boolean
---@field write_short fun(ptr: integer, value: integer): boolean
---@field write_dword fun(ptr: integer, value: integer): boolean
---@field write_qword fun(ptr: integer, value: integer): boolean
---@field write_float fun(ptr: integer, value: number): boolean
---@field write_double fun(ptr: integer, value: number): boolean
---@field read_byte fun(ptr: integer): integer
---@field read_short fun(ptr: integer): integer
---@field read_dword fun(ptr: integer): integer
---@field read_qword fun(ptr: integer): integer
---@field read_float fun(ptr: integer): number
---@field read_double fun(ptr: integer): number

---@class hb_draw
hb_draw = {}
