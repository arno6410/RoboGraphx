import svgutils

def scaleSVG(input_svg_path, scale_factor): # function to scale SVG
    # SCALE
    input_svg = svgutils.compose.SVG(input_svg_path)
    input_svg.scale(scale_factor)
    input_svg.moveto(500,3800,1)
    figure = svgutils.compose.Figure(float(input_svg.height) * scale_factor, float(input_svg.width) * scale_factor, input_svg)
    figure.save('svg_scaled.svg')
    return figure

def rotateSVG(input_svg_path, angle_deg):
    # rotate
    input_svg = svgutils.compose.SVG(input_svg_path)
    input_svg.rotate(angle_deg)
    figure = svgutils.compose.Figure(input_svg.height, input_svg.width, input_svg)
    figure.save('svg_rotated.svg')
    return figure


scaleSVG('bluetooth-svgrepo-com.svg', 5)
# rotateSVG('Belgium_brussels_iris.svg', 45)
