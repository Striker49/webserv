// main.js

// Create the scene
const scene = new THREE.Scene();

// Create a camera, which determines what we'll see when we render the scene
const camera = new THREE.PerspectiveCamera(
    75, // Field of view
    300 / 300, // Aspect ratio (1:1 for square canvas)
    0.1, // Near clipping plane
    1000 // Far clipping plane
);

// Create a renderer and attach it to our document
const renderer = new THREE.WebGLRenderer({ alpha: true }); // Set alpha to true to enable transparency
renderer.setSize(300, 300); // Set canvas size to match the cat image
document.body.appendChild(renderer.domElement);

// Create a sphere geometry
const geometry = new THREE.SphereGeometry(1, 32, 32);

// Create a basic material and set its color
const material = new THREE.MeshBasicMaterial({ color: 0x0077ff });

// Create a mesh, which is a combination of geometry and material
const sphere = new THREE.Mesh(geometry, material);

// Add the sphere to the scene
scene.add(sphere);

// Position the camera so we can see the sphere
camera.position.z = 3; // Adjusted to fit the smaller canvas

// Create an animation loop to render the scene
function animate() {
    requestAnimationFrame(animate);

    // Rotate the sphere for some basic animation
    sphere.rotation.x += 0.01;
    sphere.rotation.y += 0.01;

    renderer.render(scene, camera);
}

// Start the animation loop
animate();
