---@meta

---@class hb_draw
---@field cylinder fun(start: Vector3f, end: Vector3f, radius: number, color: integer, outline: boolean, color_outline: integer)
---@field ring fun(start: Vector3f, end: Vector3f, radius_a: number, radius_b: number, color: integer, outline: boolean, color_outline: integer)
---@field sphere fun(center: Vector3f, radius: number, color: integer, outline: boolean, color_outline: integer)
---@field box fun(pos: Vector3f, extent: Vector3f, rot: Matrix4x4f, color: integer, outline: boolean, color_outline: integer)
---@field triangle fun(pos: Vector3f, extent: Vector3f, rot: Matrix4x4f, color: integer, outline: boolean, color_outline: integer)
---@field capsule fun(start: Vector3f, end: Vector3f, radius: number, color: integer, outline: boolean, color_outline: integer)
---@field set_num_segments fun(num: integer)
---@field set_outline_tickness fun(num: integer)
---@field set_w2s fun(b: boolean)

---@class hb_draw
hb_draw = {}
