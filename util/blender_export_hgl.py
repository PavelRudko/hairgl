import bpy
import mathutils
import struct

OBJ_NAME = '<blender object name>'
PATH = '<path to the exported file>.hgl'

depsgraph = bpy.context.evaluated_depsgraph_get()
obj = depsgraph.objects.get(OBJ_NAME, None)
psys = obj.particle_systems[0]
particles = psys.particles

guides_count = len(particles)
segments_count = len(particles[0].hair_keys) - 1

vertices = []
roots_tree = mathutils.kdtree.KDTree(guides_count)
for i, particle in enumerate(particles):
    roots_tree.insert(particle.hair_keys[0].co, i)
    for hair_key in particle.hair_keys:
        vertices.append(hair_key.co)
roots_tree.balance()

triangles = []
obj.data.calc_loop_triangles()
for loop in obj.data.loop_triangles:
    for vertex_index in loop.vertices:
        co = obj.data.vertices[vertex_index].co
        root_co, root_index, dist = roots_tree.find(co)
        triangles.append(root_index)

print('Guides count:', guides_count)
print('Segments count:', segments_count)
print('Triangles count:', len(triangles) // 3)

with open(PATH, 'wb') as f:
    f.write(struct.pack('i', guides_count))
    f.write(struct.pack('i', segments_count))
    f.write(struct.pack('i', len(triangles) // 3))
    for v in vertices:
        f.write(struct.pack('f', v.x))
        f.write(struct.pack('f', v.z))
        f.write(struct.pack('f', -v.y))
    for t in triangles:
        f.write(struct.pack('i', t))
        
for i in range(0, 10):
    print(vertices[i])