




import bpy

for material in bpy.data.materials:
    if not material.use_nodes:
        continue

    node_tree = material.node_tree

    image_node = None
    bsdf_node = None

    for node in node_tree.nodes:

        if node.type == 'TEX_IMAGE':
            image_node = node
        elif node.type == 'BSDF_PRINCIPLED':
            bsdf_node = node

    if image_node and bsdf_node:
        output_socket = [socket for socket in image_node.outputs if socket.name == 'Alpha'][0]
        input_socket = [socket for socket in bsdf_node.inputs if socket.name == 'Alpha'][0]

        node_tree.links.new(input_socket, output_socket)
